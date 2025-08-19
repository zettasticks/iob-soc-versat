#include "testbench.hpp"

#define MAX_SIZE 30

void SingleTest(Arena* arena){
  int* outputBuffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);

  if(1){
    int input[] =          {1,2,3,4, 0, 0,0,0,5,6, 7, 8,0, 0, 0, 0, 9,10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,GM,GM,5,6,7,8,GM,GM,9,10,11,12,GM,GM,13,14,15,16,GM};

    ResetAccelerator();
    ClearBuffer(outputBuffer,MAX_SIZE);

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = false;
    forceSingleLoop = true;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    // +1 because we want to check that the unit did not written over the end of the buffer
    Assert_Eq(outputBuffer,expectedOutput,22 + 1,"SingleLoop");
  }

  if(1){
    int input[] =          {1,2,3,4, 0, 0,0,0,5,6, 7, 8,0, 0, 0, 0, 9,10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,GM,GM,5,6,7,8,GM,GM,9,10,11,12,GM,GM,13,14,15,16,GM};

    ResetAccelerator();
    ClearBuffer(outputBuffer,MAX_SIZE);

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = true;
    forceSingleLoop = false;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    // +1 because we want to check that the unit did not written over the end of the buffer
    Assert_Eq(outputBuffer,expectedOutput,22 + 1,"DoubleLoop");
  }

  // Make sure that VWrite does not write over data that was already there
  if(1){ // Currently disabled since we probably need to augment "duty" stuff for VWrite units.
    int input[] =          {1,2,3,4, 0, 0,0,0,5,6, 7, 8,0, 0, 0, 0, 9, 10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,GM,GM,5,6,7,8,GM,GM,9,10,11,12,GM, GM,13,14,15,16,GM};

    ResetAccelerator();
    ClearBuffer(outputBuffer,MAX_SIZE);

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = false;
    forceSingleLoop = true;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    // +1 because we want to check that the unit did not written over the end of the buffer
    Assert_Eq(outputBuffer,expectedOutput,22 + 1,"Single (Check if it does not overwrite data already present");
  }

  // Make sure that VWrite does not write over data that was already there
  if(1){
    int input[] =          {1,2,3,4, 0, 0,0,0,5,6, 7, 8,0, 0, 0, 0, 9, 10,11,12, 0, 0, 0,0,13,14,15,16};
    int expectedOutput[] = {1,2,3,4,GM,GM,5,6,7,8,GM,GM,9,10,11,12,GM, GM,13,14,15,16,GM};

    ResetAccelerator();
    ClearBuffer(outputBuffer,MAX_SIZE);

    // Input reads 4 in 4 with a stride of 4;
    // Output writes 4 in 4 with a stride of 2;
    forceDoubleLoop = true;
    forceSingleLoop = false;
    VDouble_VRead(&accelConfig->read,input,4,4,8);
    VDouble_VWrite(&accelConfig->write,outputBuffer,4,4,6);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    // +1 because we want to check that the unit did not written over the end of the buffer
    Assert_Eq(outputBuffer,expectedOutput,22 + 1,"Double (Check if it does not overwrite data already present");
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
    int expectedOutput[] = {1,2,3,4,5,6,7,8,GM};
    ResetAccelerator();
    ClearBuffer(outputBuffer,MAX_SIZE);

    forceDoubleLoop = false;
    forceSingleLoop = true;
    VTriple_VRead(&accelConfig->read,input,2,2,2,4,16);
    VLinear_VWrite(&accelConfig->write,outputBuffer,8);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,9,"SingleLoop strided");
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
    int expectedOutput[] = {1,2,3,4,5,6,7,8,GM};
    ResetAccelerator();
    ClearBuffer(outputBuffer,MAX_SIZE);

    forceDoubleLoop = true;
    forceSingleLoop = false;
    VTriple_VRead(&accelConfig->read,input,2,2,2,4,16);
    VLinear_VWrite(&accelConfig->write,outputBuffer,8);

    ClearCache(outputBuffer);
    RunAccelerator(3);
    ClearCache(outputBuffer);

    Assert_Eq(outputBuffer,expectedOutput,9,"DoubleLoop strided");
  }
}
