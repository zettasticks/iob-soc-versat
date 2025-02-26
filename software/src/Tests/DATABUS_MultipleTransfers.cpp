#include "testbench.hpp"

void SingleTest(Arena* arena){
  int input[] = {1,2,3,4,0,0,0,0,5,6,7,8,0,0,0,0,9,10,11,12,0,0,0,0,13,14,15,16};
  int expectedOutput[] = {1,2,3,4,0,0,5,6,7,8,0,0,9,10,11,12,0,0,13,14,15,16};
  int outputBuffer[] =   {0,0,0,0,0,0,0,0,0,0,0,0,0, 0, 0, 0,0,0, 0, 0, 0, 0};

  // Input reads 4 in 4 with a stride of 4;
  // Output writes 4 in 4 with a stride of 2;

  Configure_VReadMultipleLoad_VReadMultiple(&accelConfig->read,(iptr) input,4,4,8);
  Configure_VReadMultipleOutput_VReadMultiple(&accelConfig->read,4,4);
  accelConfig->read.read_enabled = 1;

  Configure_VWriteMultipleStore_VWriteMultiple(&accelConfig->write,(iptr) outputBuffer,4,4,6);
  Configure_VWriteMultipleInput_VWriteMultiple(&accelConfig->write,4,4);
  accelConfig->write.write_enabled = 1;

  RunAccelerator(3);

  for(int i = 0; i < 22; i++){
    Assert_Eq(outputBuffer[i],expectedOutput[i]);
  }
}
