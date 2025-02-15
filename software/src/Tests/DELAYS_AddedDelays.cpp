#include "testbench.hpp"

void SingleTest(Arena* arena){
   RunAccelerator(2);

   Assert_Eq(accelState->TOP_output_currentValue,15);
}