#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->input_0.constant = 5;
   RunAccelerator(3);
   Assert_Eq(accelState->output_0.currentValue,5);
 
   RunAccelerator(1);
   Assert_Eq(accelState->output_0.currentValue,10);
 
   RunAccelerator(1);
   Assert_Eq(accelState->output_0.currentValue,15);
}