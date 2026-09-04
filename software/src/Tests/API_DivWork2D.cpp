#include "testbench.hpp"

#define MAX_SIZE 2

float CalculateConv(float* features,float* weights,int size){
   float sum = 0.0f;
   for(int y = 0; y < size; y++){
      for(int x = 0; x < size; x++){
         sum += features[y*size + x] * weights[y*size + x];         
      }
   }

   return sum;
}

void SingleTest(Arena* arena){
   float features[] = {1.0f,2.0f,3.0f,
                       4.0f,5.0f,6.0f,
                       7.0f,8.0f,9.0f};

   float weights[] = {1.0f,2.0f,3.0f,
                       4.0f,5.0f,6.0f,
                       7.0f,8.0f,9.0f};

   int kernelSize = 3;

#if 0
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

   int kernelW = kernelSize;
   int kernelH = kernelSize;

   int stride = kernelW * kernelH;

   API_DivWork2D_Conv(features,kernelW,kernelH,kernelSize);
   API_DivWork2D_Weight(weights,kernelW,kernelH);
   API_DivWork2D_Out(buffer,1,stride);

   accelConfig->myAccum.strideMinusOne = stride - 1;

   RunAccelerator(3);
   ClearCache(buffer);

   float expected = CalculateConv(features,weights,kernelSize);
   printf("%f %f\n",expected,buffer[0]);
   Assert_Eq(expected,buffer[0]);
}
