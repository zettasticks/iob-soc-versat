#include "testbench.hpp"
#include "unitConfiguration.hpp"

#include <cmath>

#define IMAGE_INPUT_W 416
#define IMAGE_INPUT_C 3 // Channels

#define KERNEL_SIZE 3

// Some of this data is given, the other is initialized afterwards
enum LayerType{
   LayerType_CONV,
   LayerType_MAX,
   LayerType_YOLO,
   LayerType_ROUTE,
   LayerType_UPSAMPLE
};

enum ActivationType{
   ActivationType_LEAKY,
   ActivationType_LINEAR,
   ActivationType_SIGMOID
};

struct LayerInfo{
   LayerType type;

   union{
      struct{
         int inputW;
         int inputC;
      };
      struct{
         int routeLayer0;
         int routeLayer1;
      };
   };

   int outputW;
   int outputC;

   // Data needed by CONV and YOLO
   int kernelS;

   int isNormalized; 
   ActivationType activationType;

   // Data that is filled later.
   Array<float> weights;
   Array<float> bias;
   Array<float> scale;
   Array<float> rolling_mean;
   Array<float> rolling_variance;
};

// Tiny YOLO V3
static LayerInfo layerInfo[] = {
   //                   IW   IC  OW   OC K N 
   {LayerType_CONV,    416,   3,416,  16,3,1,ActivationType_LEAKY},  // 1
   {LayerType_MAX,     416,  16,208,  16},                           // 2
   {LayerType_CONV,    208,  16,208,  32,3,1,ActivationType_LEAKY},  // 3
   {LayerType_MAX,     208,  32,104,  32},                           // 4
   {LayerType_CONV,    104,  32,104,  64,3,1,ActivationType_LEAKY},  // 5
   {LayerType_MAX,     104,  64, 52,  64},                           // 6
   {LayerType_CONV,     52,  64, 52, 128,3,1,ActivationType_LEAKY},  // 7
   {LayerType_MAX,      52, 128, 26, 128},                           // 8
   {LayerType_CONV,     26, 128, 26, 256,3,1,ActivationType_LEAKY},  // 9
   {LayerType_MAX,      26, 256, 13, 256},                           // 10
   {LayerType_CONV,     13, 256, 13, 512,3,1,ActivationType_LEAKY},  // 11
   {LayerType_MAX,      13, 512, 13, 512},                           // 12
   {LayerType_CONV,     13, 512, 13,1024,3,1,ActivationType_LEAKY},  // 13
   {LayerType_CONV,     13,1024, 13, 256,1,1,ActivationType_LEAKY},  // 14
   {LayerType_CONV,     13, 256, 13, 512,3,1,ActivationType_LEAKY},  // 15
   {LayerType_CONV,     13, 512, 13, 255,1,0,ActivationType_LINEAR}, // 16
   {LayerType_YOLO,     13, 255, 13, 255,0,0,ActivationType_SIGMOID},// 17
   {LayerType_ROUTE,    14,  14, 13, 256},                           // 18
   {LayerType_CONV,     13, 256, 13, 128,1,1,ActivationType_LEAKY},  // 19
   {LayerType_UPSAMPLE, 13, 128, 26, 128},                           // 20
   {LayerType_ROUTE,     9,  20, 26, 384},                           // 21
   {LayerType_CONV,     26, 384, 26, 256,3,1,ActivationType_LEAKY},  // 22
   {LayerType_CONV,     26, 256, 26, 255,1,0,ActivationType_LINEAR}, // 23
   {LayerType_YOLO,     26, 255, 26, 255,0,0,ActivationType_SIGMOID} // 24
};

inline int Kernel2DChannels(LayerInfo& info){return info.inputC;};
inline int Kernel3DChannels(LayerInfo& info){return info.outputC;};
inline int Kernel2DSize(LayerInfo& info){return info.kernelS * info.kernelS;};
inline int Kernel3DSize(LayerInfo& info){return Kernel2DSize(info) * Kernel2DChannels(info);};
inline int KernelTotalSize(LayerInfo& info){return Kernel3DSize(info) * Kernel3DChannels(info);};

inline int BiasAmount(LayerInfo& info){return Kernel3DChannels(info);};

inline int LayerUsesBias(LayerInfo& info){return (info.type == LayerType_CONV);};

#include <iosfwd>

extern "C" FILE* fopen(const char* filename,const char* mode);
extern "C" size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern "C" size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern "C" size_t fclose(FILE *stream);

struct Layer{
   LayerInfo* layerInfoThatGeneratedThis;

   Array<Array<float>> channels; // The channels should be packed together. This happens naturally right now, but should be enforced
};

String ReadFilePC(Arena* arena,const char* filepath){
   FILE* f = fopen(filepath,"r"); 

   char* start = (char*) PushBytes(arena,0);
   int read = fread(start,sizeof(char),arena->totalAllocated - arena->used,f);
   Array<char> file = PushArray<char>(arena,read + 1);
   file[read] = '\0';

   fclose(f);

   return (String){.str = file.data,.size = read};
}

void WriteImagePC(const char* filepath,Layer layer){
   float* start = &layer.channels[0][0]; // Assuming that everything is straing inside the layer.
   FILE* f = fopen(filepath,"w"); 

   fwrite(start,sizeof(float),416 * 416 * 3,f);

   fclose(f);
}  

float GetValue(Array<float> channel,int channelW,int x,int y){
   if(x < 0 || x >= channelW){
      return 0.0f;
   }
   if(y < 0 || y >= channelW){
      return 0.0f;
   }

   return channel[y * channelW + x];
}

Array<float> Get3DKernelOffChannel(LayerInfo& info,int inputChannel){
   int kernel3DSize = Kernel3DSize(info);

   Array<float> res = {};
   res.data = info.weights.data + (kernel3DSize * inputChannel);
   res.size = kernel3DSize;

   return res;
}

Array<float> Get2DKernelOffChannel(LayerInfo& info,Array<float> kernel3D,int outputChannel){
   int kernel2DSize = Kernel2DSize(info);

   Array<float> res = {};
   res.data = kernel3D.data + (kernel2DSize * outputChannel);
   res.size = kernel2DSize;

   return res;
}

#define MAX(A,B) (((A) > (B)) ? (A) : (B))

Layer PerformMaxPool(Arena* out,Layer input,LayerInfo& info){
   Layer result = {};
   result.layerInfoThatGeneratedThis = &info;
   
   assert(info.type == LayerType_MAX);

   result.channels = PushArray<Array<float>>(out,info.outputC);
   for(int i = 0; i < info.outputC; i++){
      result.channels[i] = PushArray<float>(out,info.outputW * info.outputW);
   }

   int kernelW = 2;
   int stride = info.inputW / info.outputW;

   printf("Stride:%d\n",stride);

   if(stride == 2){
      for(int outC = 0; outC < info.outputC; outC++){
         Array<float> out = result.channels[outC];
         Array<float> in = input.channels[outC];

         assert(in.size == out.size * stride * stride);

         for(int oY = 0; oY < info.outputW; oY++){
            for(int oX = 0; oX < info.outputW; oX++){
               int iY0 = oY * 2;
               int iY1 = oY * 2 + 1;
               int iX0 = oX * 2;
               int iX1 = oX * 2 + 1;

               float topLeft     = in[iY0 * info.inputW + iX0];
               float topRight    = in[iY0 * info.inputW + iX1];
               float bottomLeft  = in[iY1 * info.inputW + iX0];
               float bottomRight = in[iY1 * info.inputW + iX1];

               out[oY * info.outputW + oX] = MAX(topLeft,MAX(topRight,MAX(bottomLeft,bottomRight)));
            }
         }
      }
   } else if(stride == 1){
      for(int outC = 0; outC < info.outputC; outC++){
         Array<float> out = result.channels[outC];
         Array<float> in = input.channels[outC];

         assert(in.size == out.size * stride * stride);

         for(int oY = 0; oY < info.outputW; oY++){
            for(int oX = 0; oX < info.outputW; oX++){
               int iY0 = oY;
               int iY1 = oY + 1;
               int iX0 = oX;
               int iX1 = oX + 1;

               float topLeft     = in[iY0 * info.inputW + iX0];
               float topRight    = iX1 >= info.inputW ? 0.0f : in[iY0 * info.inputW + iX1];
               float bottomLeft  = iY1 >= info.inputW ? 0.0f : in[iY1 * info.inputW + iX0];
               float bottomRight = (iY1 >= info.inputW || iX1 >= info.inputW) ? 0.0f : in[iY1 * info.inputW + iX1];

               out[oY * info.outputW + oX] = MAX(topLeft,MAX(topRight,MAX(bottomLeft,bottomRight)));
            }
         }
      }      
   } else {
      assert(false);
   }

   return result;
}

Layer PerformUpsample(Arena* out,Layer input,LayerInfo& info){
   Layer result = {};
   result.layerInfoThatGeneratedThis = &info;

   assert(info.type == LayerType_UPSAMPLE);

   result.channels = PushArray<Array<float>>(out,info.outputC);
   for(int i = 0; i < info.outputC; i++){
      result.channels[i] = PushArray<float>(out,info.outputW * info.outputW);
   }

   int upsampleFactor = info.outputW / info.inputW;
   for(int outC = 0; outC < info.outputC; outC++){
      Array<float> out = result.channels[outC];
      Array<float> in = input.channels[outC];

      assert(out.size == in.size * upsampleFactor * upsampleFactor);

      for(int iY = 0; iY < info.inputW; iY++){
         for(int iX = 0; iX < info.inputW; iX++){
            int oY0 = iY * 2;
            int oY1 = iY * 2 + 1;
            int oX0 = iX * 2;
            int oX1 = iX * 2 + 1;

            float value = in[iY * info.inputW + iX];

            out[oY0 * info.outputW + oX0] = value;
            out[oY0 * info.outputW + oX1] = value;
            out[oY1 * info.outputW + oX0] = value;
            out[oY1 * info.outputW + oX1] = value;
         }
      }
   }
      
   return result;
}

Layer PerformConvolution(Arena* out,Layer input,LayerInfo& info){
   Layer result = {};
   result.layerInfoThatGeneratedThis = &info;

   assert(info.type == LayerType_CONV);

   result.channels = PushArray<Array<float>>(out,info.outputC);
   for(int i = 0; i < info.outputC; i++){
      result.channels[i] = PushArray<float>(out,info.outputW * info.outputW);
   }

   for(int outC = 0; outC < info.outputC; outC++){
      Array<float> kernel3D = Get3DKernelOffChannel(info,outC);
      Array<float> out = result.channels[outC];

      for(int i = 0; i < out.size; i++){
         out[i] = 0;
      }

      for(int oY = 0; oY < info.outputW; oY++){
         for(int oX = 0; oX < info.outputW; oX++){
         
            for(int inC = 0; inC < info.inputC; inC++){
               Array<float> in = input.channels[inC];
               Array<float> kernel = Get2DKernelOffChannel(info,kernel3D,inC);

               float sum;
               if(kernel.size == 9){
                  float topLeft     = GetValue(in,info.inputW,oX - 1,oY - 1) * kernel[0];
                  float top         = GetValue(in,info.inputW,oX    ,oY - 1) * kernel[1];
                  float topRight    = GetValue(in,info.inputW,oX + 1,oY - 1) * kernel[2];
                  float centerLeft  = GetValue(in,info.inputW,oX - 1,oY)     * kernel[3];
                  float center      = GetValue(in,info.inputW,oX    ,oY)     * kernel[4];
                  float centerRight = GetValue(in,info.inputW,oX + 1,oY)     * kernel[5];
                  float bottomLeft  = GetValue(in,info.inputW,oX - 1,oY + 1) * kernel[6];
                  float bottom      = GetValue(in,info.inputW,oX    ,oY + 1) * kernel[7];
                  float bottomRight = GetValue(in,info.inputW,oX + 1,oY + 1) * kernel[8];

                  sum = topLeft + top + topRight + centerLeft + center + centerRight + bottomLeft + bottom + bottomRight;
               } else if(kernel.size == 1){
                  sum = GetValue(in,info.inputW,oX,oY) * kernel[0];
               }

               out[oY * info.outputW + oX] += sum;
            }
         }
      }
   }

#if 1
   // Adds bias
   for(int outC = 0; outC < info.outputC; outC++){
      Array<float> out = result.channels[outC];
      float bias = info.bias[outC];

      for(int i = 0; i < out.size; i++){
         out[i] += bias;
      }
   }
#endif

#if 1
   // Performs activation 
   for(int outC = 0; outC < info.outputC; outC++){
      Array<float> out = result.channels[outC];

      for(int i = 0; i < out.size; i++){
         if(info.activationType == ActivationType_LINEAR){

         } else if(info.activationType == ActivationType_LEAKY){
            if(out[i] < 0.0f){
               out[i] *= 0.1f;
            }
         }
      }
   }
#endif

   return result;
}

Layer PerformYOLO(Arena* out,Layer input,LayerInfo& info){
   Layer result = {};
   result.layerInfoThatGeneratedThis = &info;

   assert(info.type == LayerType_YOLO);

   result.channels = PushArray<Array<float>>(out,info.outputC);
   for(int i = 0; i < info.outputC; i++){
      result.channels[i] = PushArray<float>(out,info.outputW * info.outputW);
   }

   for(int c = 0; c < info.outputC; c++){
      Array<float> in = input.channels[c]; 
      Array<float> output = result.channels[c];

      for(int xy = 0; xy < info.outputW * info.outputW; xy++){
         if(c == 2 || c == 3 || c == 87 || c == 88 || c == 172 || c == 173){
            output[xy] = in[xy];
            continue;
         }

         // Need to perform sigmoid linear.
         output[xy] = 1.0f / (1.0f + exp(-in[xy]));

         #if 0
         if(in[xy] <= -5.0f){
            output[xy] = 0.0f;
         } else if(in[xy] <= -2.375) {
            output[xy] = in[xy] * 0.03125f + 0.15625f;
         } else if(in[xy] <= -1) {
            output[xy] = in[xy] * 0.125f + 0.375f;
         } else if(in[xy] <= 1) {
            output[xy] = in[xy] * 0.25f + 0.5f;
         } else if(in[xy] <= 2.375) {
            output[xy] = in[xy] * 0.125f + 0.0625f;
         } else if(in[xy] <= 5) {
            output[xy] = in[xy] * 0.03125f + 0.84375f;
         } else {
            output[xy] = 1.0f;
         } 
         #endif
         //printf("%f -> %f\n",in[xy],output[xy]);
      }
   }

   return result;
}

Layer PerformConcatenation(Arena* out,Layer first,Layer second,LayerInfo& info){
   Layer result = {};
   result.layerInfoThatGeneratedThis = &info;

   assert(info.type == LayerType_ROUTE);

   result.channels = PushArray<Array<float>>(out,info.outputC);
   for(int i = 0; i < info.outputC; i++){
      result.channels[i] = PushArray<float>(out,info.outputW * info.outputW);
   }

   int outChannel = 0;
   for(int c = 0; c < first.channels.size; c++){
      Array<float> in = first.channels[c];

      memcpy(result.channels[outChannel++].data,in.data,in.size * sizeof(float));
   }
   for(int c = 0; c < second.channels.size; c++){
      Array<float> in = second.channels[c];

      memcpy(result.channels[outChannel++].data,in.data,in.size * sizeof(float));
   }
   return result;
}

struct ClassBox{
   // Normalized in image space
   float x;
   float y;
   float width;
   float height;
};


struct BoundingBox{
// All this values are collected straight from the result of YOLO stages without any processing.
   float x;
   float y;
   float sizeX;
   float sizeY;
   float objectnessScore;
   float classScores[80];
// Extra data associated to the box
   int gridX;
   int gridY;
   int gridSize;
   int channel;
   int yoloGridLayer;
};

ClassBox TransformBoundingBoxToClass(BoundingBox box){
   float gridSize = (float) box.gridSize;
   float gridTop  = (float) (box.gridY * box.gridSize);
   float gridLeft = (float) (box.gridX * box.gridSize);

   ClassBox res = {};

   float yolo_bias[12] = {10.0f, 14.0f, 23.0f, 27.0f, 37.0f, 58.0f, 81.0f, 82.0f, 135.0f, 169.0f, 344.0f, 319.0f};

   int n = box.channel + box.yoloGridLayer * 3; // Para x:y = 9:2

   float lw = 26.0f;
   if(box.yoloGridLayer == 1){
      lw = 13.0f;
   }

   res.x = (box.gridX + box.x) / lw;
   res.y = (box.gridY + box.y) / lw;
   res.width  = (exp(box.sizeX) * yolo_bias[2 * n]) / 416.0f;
   res.height = (exp(box.sizeY) * yolo_bias[2 * n + 1]) / 416.0f;

   printf("%d %d %d %d %f %f %f %f\n",box.channel,box.yoloGridLayer,box.gridX,box.gridY,res.x,res.y,res.width,res.height);

   return res;
}

void DrawBox(Layer input,ClassBox box){
   int imageW = 416;

   int centerX = (int) (box.x * imageW);
   int centerY = (int) (box.y * imageW);
   int halfWidth = (int) (box.width * imageW / 2.0f);
   int halfHeight = (int) (box.height * imageW / 2.0f);

   int left  = centerX - halfWidth;
   int right = centerX + halfWidth;

   int top = centerY - halfHeight;
   int bottom = centerY + halfHeight;

   if(left < 0) left = 0;
   if(right >= 416) right = 415;

   if(top < 0) top = 0;
   if(bottom >= 416) bottom = 415;

   for(int x = left; x <= right; x++){
      input.channels[0][top * imageW + x] = 1.0f;
      input.channels[0][bottom * imageW + x] = 1.0f;
   }

   for(int y = top; y <= bottom; y++){
      input.channels[0][y * imageW + left] = 1.0f;
      input.channels[0][y * imageW + right] = 1.0f;
   }
}

Array<BoundingBox> GetBoundingBoxes(Arena* out,Layer result,int gridSize,int yoloGridLayer){
   int boxW = 85;
   assert(sizeof(BoundingBox) >= sizeof(float) * boxW);

   Byte* mark = MarkArena(out);

   for(int y = 0; y < gridSize; y++){
      for(int x = 0; x < gridSize; x++){
         int xy = y * gridSize + x;
         for(int i = 0; i < 3; i++){
            BoundingBox box = {};
            float* view = (float*) &box;

            // Loads box;
            for(int j = 0; j < boxW; j++){
               view[j] = result.channels[i * boxW + j][xy];
            }

            if(box.objectnessScore < 0.25){
               continue;
            }

            box.gridX = x;
            box.gridY = y;
            box.gridSize = gridSize;
            box.channel = i;
            box.yoloGridLayer = yoloGridLayer;
            *PushType<BoundingBox>(out) = box;
         }
      }
   }

   Array<BoundingBox> res = PushArray<BoundingBox>(out,mark);
   return res;
}

void PrintSomeValues(Layer res,LayerInfo& info){
   int w = info.outputW;
   printf("Channel 0\n");
   int c = 0;
   printf("%f %f %f %f\n",res.channels[c][0 * w + 0],res.channels[c][0 * w + 1],res.channels[c][0 * w + 2],res.channels[c][0 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][1 * w + 0],res.channels[c][1 * w + 1],res.channels[c][1 * w + 2],res.channels[c][1 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][2 * w + 0],res.channels[c][2 * w + 1],res.channels[c][2 * w + 2],res.channels[c][2 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][3 * w + 0],res.channels[c][3 * w + 1],res.channels[c][3 * w + 2],res.channels[c][3 * w + 3]);

   printf("Channel 1\n");
   c = 1;
   printf("%f %f %f %f\n",res.channels[c][0 * w + 0],res.channels[c][0 * w + 1],res.channels[c][0 * w + 2],res.channels[c][0 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][1 * w + 0],res.channels[c][1 * w + 1],res.channels[c][1 * w + 2],res.channels[c][1 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][2 * w + 0],res.channels[c][2 * w + 1],res.channels[c][2 * w + 2],res.channels[c][2 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][3 * w + 0],res.channels[c][3 * w + 1],res.channels[c][3 * w + 2],res.channels[c][3 * w + 3]);

   printf("Channel 2\n");
   c = 2;
   printf("%f %f %f %f\n",res.channels[c][0 * w + 0],res.channels[c][0 * w + 1],res.channels[c][0 * w + 2],res.channels[c][0 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][1 * w + 0],res.channels[c][1 * w + 1],res.channels[c][1 * w + 2],res.channels[c][1 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][2 * w + 0],res.channels[c][2 * w + 1],res.channels[c][2 * w + 2],res.channels[c][2 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][3 * w + 0],res.channels[c][3 * w + 1],res.channels[c][3 * w + 2],res.channels[c][3 * w + 3]);

   printf("Channel 3\n");
   c = 3;
   printf("%f %f %f %f\n",res.channels[c][0 * w + 0],res.channels[c][0 * w + 1],res.channels[c][0 * w + 2],res.channels[c][0 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][1 * w + 0],res.channels[c][1 * w + 1],res.channels[c][1 * w + 2],res.channels[c][1 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][2 * w + 0],res.channels[c][2 * w + 1],res.channels[c][2 * w + 2],res.channels[c][2 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][3 * w + 0],res.channels[c][3 * w + 1],res.channels[c][3 * w + 2],res.channels[c][3 * w + 3]);

   printf("Channel 4\n");
   c = 4;
   printf("%f %f %f %f\n",res.channels[c][0 * w + 0],res.channels[c][0 * w + 1],res.channels[c][0 * w + 2],res.channels[c][0 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][1 * w + 0],res.channels[c][1 * w + 1],res.channels[c][1 * w + 2],res.channels[c][1 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][2 * w + 0],res.channels[c][2 * w + 1],res.channels[c][2 * w + 2],res.channels[c][2 * w + 3]);
   printf("%f %f %f %f\n",res.channels[c][3 * w + 0],res.channels[c][3 * w + 1],res.channels[c][3 * w + 2],res.channels[c][3 * w + 3]);
}

void SingleTest(Arena* arena){
   String weightsContent = ReadFilePC(arena,"../../yolov3-tiny.weights");

   Arena weightsInst = {.mem = (Byte*) weightsContent.str,
                        .totalAllocated = weightsContent.size};

   Arena* data = &weightsInst;

   int* major = PushType<int>(data);
   int* minor = PushType<int>(data);
   int* revision = PushType<int>(data);
   uint64_t* iseen = PushType<uint64_t>(data);

   for(int i = 0; i < ARRAY_SIZE(layerInfo); i++){
      LayerInfo& info = layerInfo[i];
      if(LayerUsesBias(info)){
         int amountOfBias = BiasAmount(info);

         info.bias = PushArray<float>(data,amountOfBias);
         if(info.isNormalized){
            info.scale = PushArray<float>(data,amountOfBias);
            info.rolling_mean = PushArray<float>(data,amountOfBias);
            info.rolling_variance = PushArray<float>(data,amountOfBias);
         }
      }

      info.weights = PushArray<float>(data,KernelTotalSize(info));

#if 1
      // Batches normalization into weights and bias
      if(LayerUsesBias(info) && info.isNormalized){
         int amountOfBias = BiasAmount(info);
         int filter_size = Kernel3DSize(info);

         for (int f = 0; f < amountOfBias; ++f)
         {
            info.bias[f] = info.bias[f] - (double)info.scale[f] * info.rolling_mean[f] / (sqrt((double)info.rolling_variance[f] + .00001));
            double precomputed = info.scale[f] / (sqrt((double)info.rolling_variance[f] + .00001));

            for (int j = 0; j < filter_size; ++j)
            {
               int w_index = f*filter_size + j;

               info.weights[w_index] *= precomputed;
            }
         }
      }
#endif
   }

   Assert_Eq(data->totalAllocated,data->used);

   String imageContent = ReadFilePC(arena,"../../output.rgb");

   Arena imageInst = {.mem = (Byte*) imageContent.str,
                     .totalAllocated = imageContent.size};

   Arena* image = &imageInst;

   Layer beginning = {};
   beginning.channels = PushArray<Array<float>>(arena,3);
   for(int i = 0; i < 3; i++){
      Array<float> channelData = PushArray<float>(image,416 * 416); 
      beginning.channels[i] = channelData;
   }   

   Assert_Eq(image->totalAllocated,image->used);

   printf("\n");

   // I think that both weights and bias are correct reaching here. They have the normalization batched into them.
   // 
   Layer result1 = PerformConvolution(arena,beginning,layerInfo[0]);

   printf("\n%d\n",0);
   PrintSomeValues(result1,layerInfo[0]);

   Layer result2  = PerformMaxPool(arena,result1,layerInfo[1]);
   printf("\n%d\n",1);
   PrintSomeValues(result2,layerInfo[1]);

   Layer result3  = PerformConvolution(arena,result2,layerInfo[2]);

   printf("\n%d\n",2);
   PrintSomeValues(result3,layerInfo[2]);

   Layer result4  = PerformMaxPool(arena,result3,layerInfo[3]);

   Layer result5  = PerformConvolution(arena,result4,layerInfo[4]);
   printf("\n%d\n",4);
   PrintSomeValues(result5,layerInfo[4]);

   Layer result6  = PerformMaxPool(arena,result5,layerInfo[5]);

   Layer result7  = PerformConvolution(arena,result6,layerInfo[6]);
   Layer result8  = PerformMaxPool(arena,result7,layerInfo[7]);

   Layer result9  = PerformConvolution(arena,result8,layerInfo[8]);
   Layer result10 = PerformMaxPool(arena,result9,layerInfo[9]);

   Layer result11 = PerformConvolution(arena,result10,layerInfo[10]);
   Layer result12 = PerformMaxPool(arena,result11,layerInfo[11]);

   Layer result13 = PerformConvolution(arena,result12,layerInfo[12]);
   Layer result14 = PerformConvolution(arena,result13,layerInfo[13]);
   Layer result15 = PerformConvolution(arena,result14,layerInfo[14]);
   Layer result16 = PerformConvolution(arena,result15,layerInfo[15]);
   printf("\n%d\n",15);
   PrintSomeValues(result16,layerInfo[15]);

   Layer result17 = PerformYOLO(arena,result16,layerInfo[16]);
   printf("\n%d\n",16);
   PrintSomeValues(result17,layerInfo[16]);

   Layer result18 = result14;

   Layer result19 = PerformConvolution(arena,result18,layerInfo[18]);
   printf("\n%d\n",18);
   PrintSomeValues(result19,layerInfo[18]);

   Layer result20 = PerformUpsample(arena,result19,layerInfo[19]);
   printf("\n%d\n",19);
   PrintSomeValues(result20,layerInfo[19]);

   Layer result21 = PerformConcatenation(arena,result20,result9,layerInfo[20]);
   printf("\n%d\n",20);
   PrintSomeValues(result21,layerInfo[20]);

   Layer result22 = PerformConvolution(arena,result21,layerInfo[21]);
   printf("\n%d\n",21);
   PrintSomeValues(result22,layerInfo[21]);

   Layer result23 = PerformConvolution(arena,result22,layerInfo[22]);
   printf("\n%d\n",22);
   PrintSomeValues(result23,layerInfo[22]);

   // TODO: Probably gonna need to output data from YOLO and compare with ours.
   //       It does not appear to match up.
 
   Layer result24 = PerformYOLO(arena,result23,layerInfo[23]);
   printf("\n%d\n",23);
   PrintSomeValues(result24,layerInfo[23]);

   printf("\n\n");

   Byte* mark = MarkArena(arena);
   GetBoundingBoxes(arena,result17,13,1);
   GetBoundingBoxes(arena,result24,26,0);
   Array<BoundingBox> boxes = PushArray<BoundingBox>(arena,mark);

   for(BoundingBox b : boxes){
      auto c = TransformBoundingBoxToClass(b);
      DrawBox(beginning,c); // We should copy and not change beginning
      //printf("%f %f %f %f\n",c.x,c.y,c.width,c.height);
   }

   WriteImagePC("test.rgb",beginning);
}
