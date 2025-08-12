#include "testbench.hpp"

void SingleTest(Arena* arena){
  int* outputBuffer = (int*) PushBytes(arena,sizeof(int) * 20);

  if(1){
    int input[] =          {1,2,3,4,0,0,0,0,5,6,7,8,0, 0, 0, 0,9,10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,0,0,5,6,7,8,0,0,9,10,11,12,0, 0,13,14,15,16};

    ResetAccelerator();
    ClearBuffer(outputBuffer,20);

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = false;
    forceSingleLoop = true;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,22,"SingleLoop");
  }

  if(1){
    int input[] =          {1,2,3,4,0,0,0,0,5,6,7,8,0, 0, 0, 0,9,10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,0,0,5,6,7,8,0,0,9,10,11,12,0, 0,13,14,15,16};

    ClearBuffer(outputBuffer,20);

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = true;
    forceSingleLoop = false;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,22,"DoubleLoop");
  }

  // Make sure that VWrite does not write over data that was already there
  if(0){ // Currently disabled since we probably need to augment "duty" stuff for VWrite units.
    int input[] =          {1,2,3,4, 0, 0,0,0,5,6, 7, 8,0, 0, 0, 0, 9, 10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,-1,-1,5,6,7,8,-1,-1,9,10,11,12,-1, -1,13,14,15,16};

    ClearBuffer(outputBuffer,20);

    outputBuffer[4] = -1;
    outputBuffer[5] = -1;
    outputBuffer[10] = -1;
    outputBuffer[11] = -1;
    outputBuffer[10] = -1;
    outputBuffer[11] = -1;
    outputBuffer[16] = -1;
    outputBuffer[17] = -1;

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = false;
    forceSingleLoop = true;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,22,"Single (Check if it does not overwrite data already present");
  }

  // Make sure that VWrite does not write over data that was already there
  if(1){
    int input[] =          {1,2,3,4, 0, 0,0,0,5,6, 7, 8,0, 0, 0, 0, 9, 10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,-1,-1,5,6,7,8,-1,-1,9,10,11,12,-1, -1,13,14,15,16};

    ClearBuffer(outputBuffer,20);

    outputBuffer[4] = -1;
    outputBuffer[5] = -1;
    outputBuffer[10] = -1;
    outputBuffer[11] = -1;
    outputBuffer[10] = -1;
    outputBuffer[11] = -1;
    outputBuffer[16] = -1;
    outputBuffer[17] = -1;

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = true;
    forceSingleLoop = false;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,22,"Double (Check if it does not overwrite data already present");
  }

  if(1){
    int input[] = {1,2,0,0,
                   3,4,0,0,
                   0,0,0,0,
                   0,0,0,0,
                   5,6,0,0,
                   7,8,0,0,
                   0,0,0,0,
                   0,0,0,0};
    int expectedOutput[] = {1,2,3,4,5,6,7,8,0};
    ClearBuffer(outputBuffer,10);

    forceDoubleLoop = false;
    forceSingleLoop = true;
    VTriple_VRead(&accelConfig->read,input,2,2,2,4,16);
    VLinear_VWrite(&accelConfig->write,outputBuffer,8);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,8,"SingleLoop strided");
  }


  if(1){
    int input[] = {1,2,0,0,
                   3,4,0,0,
                   0,0,0,0,
                   0,0,0,0,
                   5,6,0,0,
                   7,8,0,0,
                   0,0,0,0,
                   0,0,0,0};
    int expectedOutput[] = {1,2,3,4,5,6,7,8,0};
    ClearBuffer(outputBuffer,10);

    forceDoubleLoop = true;
    forceSingleLoop = false;
    VTriple_VRead(&accelConfig->read,input,2,2,2,4,16);
    VLinear_VWrite(&accelConfig->write,outputBuffer,8);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,8,"DoubleLoop strided");
  }
}
