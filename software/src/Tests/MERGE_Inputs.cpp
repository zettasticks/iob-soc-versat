#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->input_0.constant = 10;
   accelConfig->input_1.constant = 5;

   ActivateMergedAccelerator(MergeType_TestMergeInputs1);

   RunAccelerator(3);

   Assert_Eq(accelState->output_0.currentValue,15);

   ActivateMergedAccelerator(MergeType_TestMergeInputs2);

   RunAccelerator(3);

   Assert_Eq(accelState->output_0.currentValue,5);
}