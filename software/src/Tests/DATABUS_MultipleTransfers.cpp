#include "testbench.hpp"

void SingleTest(Arena* arena){
  {
    int input[] =          {1,2,3,4,0,0,0,0,5,6,7,8,0, 0, 0, 0,9,10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,0,0,5,6,7,8,0,0,9,10,11,12,0, 0,13,14,15,16};

    int* outputBuffer = (int*) PushBytes(arena,sizeof(int) * 20);
    memset(outputBuffer,0,sizeof(int) * 20);

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    RunAccelerator(3);
    ClearCache(outputBuffer);

    for(int i = 0; i < 22; i++){
      Assert_Eq(outputBuffer[i],expectedOutput[i]);
    }
  }

  {
    int input[] = {1,2,0,0,
                   3,4,0,0,
                   0,0,0,0,
                   0,0,0,0,
                   5,6,0,0,
                   7,8,0,0,
                   0,0,0,0,
                   0,0,0,0};
    int expectedOutput[] = {1,2,3,4,5,6,7,8,0};
    int* outputBuffer = (int*) PushBytes(arena,sizeof(int) * 10);
    memset(outputBuffer,0,sizeof(int) * 10);

    VTriple_VRead(&accelConfig->read,input,2,2,2,4,16);
    VLinear_VWrite(&accelConfig->write,outputBuffer,8);

    RunAccelerator(3);
    ClearCache(outputBuffer);

    for(int i = 0; i < 8; i++){
      Assert_Eq(outputBuffer[i],expectedOutput[i]);
    }
  }
}
