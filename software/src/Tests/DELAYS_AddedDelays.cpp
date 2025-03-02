#include "testbench.hpp"

void SingleTest(Arena* arena){
   RunAccelerator(2);

   Assert_Eq(accelState->output.currentValue,15);
}