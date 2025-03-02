#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelStatic->CONFIG_OnlyStatic_unit_constant = 42;

   RunAccelerator(3);

   Assert_Eq(accelState->output.currentValue,42);
}