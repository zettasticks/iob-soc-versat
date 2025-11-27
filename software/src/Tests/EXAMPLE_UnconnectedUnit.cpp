#include "testbench.hpp"

void SingleTest(Arena* arena){
   int value = 123;

   accelConfig->unit.address = (iptr) &value;

   RunAccelerator(3);

   Assert_Eq(123,accelState->unit.lastRead);

   value = 321;

   RunAccelerator(3);

   Assert_Eq(321,accelState->unit.lastRead);
}