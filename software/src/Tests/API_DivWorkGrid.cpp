#include "testbench.hpp"

#define MAX_SIZE 2

void CalculateConv(float* output,float* features,float* weights,int size,int imageSize,int startX){
   float sum = 0.0f;
   for(int y = 0; y < size; y++){
      for(int x = 0; x < size; x++){
         sum += features[y*imageSize + (x + startX)] * weights[y*size + x];
      }
   }
   output[startX] = sum;
}

void SingleTest(Arena* arena){
#if 0
   float features[] = {1.0f,2.0f,3.0f,
                       4.0f,5.0f,6.0f,
                       7.0f,8.0f,9.0f};

   float weights[] = {1.0f,2.0f,3.0f,
                       4.0f,5.0f,6.0f,
                       7.0f,8.0f,9.0f};
#endif

   int kernelSize = 4;
   int imageSize = 4;
   int xSize = 1;

#if 1
   float features[] = {1.0f,2.0f,3.0f,4.0f,
                       5.0f,6.0f,7.0f,8.0f,
                       9.0f,10.0f,11.0f,12.0f,
                       13.0f,14.0f,15.0f,16.0f};

   float weights[] = {1.0f,2.0f,3.0f,4.0f,
                       5.0f,6.0f,7.0f,8.0f,
                       9.0f,10.0f,11.0f,12.0f,
                       13.0f,14.0f,15.0f,16.0f};
#endif

   float* buffer = (float*) PushBytes(arena,sizeof(float) * 2);
   ClearBuffer((int*) buffer,2);

  float* buffer2 = (float*) PushBytes(arena,sizeof(float) * 2);
   ClearBuffer((int*) buffer,2);

   int kernelW = kernelSize;
   int kernelH = kernelSize;

   int stride = 4;

   API_DivWorkGrid_Conv(features,kernelW,kernelH,imageSize,xSize);
   API_DivWorkGrid_Weight(weights,kernelW,kernelH,xSize);
   API_DivWorkGrid_Out(buffer,2,kernelW,kernelH);

   RunAccelerator(3);
   ClearCache(buffer);

   for(int x = 0; x < xSize; x++){
      CalculateConv(buffer2,features,weights,kernelSize,imageSize,x);
   }
   CalculateConv(buffer2,features,weights,kernelSize,imageSize,0);

   for(int x = 0; x < xSize; x++){
      printf("%f %f\n",buffer2[x],buffer[x]);
      Assert_Eq(buffer2[x],buffer[x]);
   }
}
