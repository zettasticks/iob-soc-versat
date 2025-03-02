#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->constant.constant = 5;
   RunAccelerator(2);

   Assert_Eq(accelState->output.currentValue,5);
}