#include "testbench.hpp"

void SingleTest(Arena* arena){
   int value = 123;

   accelConfig->EXAMPLE_UnconnectedUnit.unit.address = (iptr) &value;

   RunAccelerator(3);

   Assert_Eq(123,accelState->EXAMPLE_UnconnectedUnit.unit.lastRead);

   value = 321;

   RunAccelerator(3);

   Assert_Eq(321,accelState->EXAMPLE_UnconnectedUnit.unit.lastRead);

   int* data = (int*) PushBytes(arena,8);

   accelConfig->EXAMPLE_UnconnectedUnit2.unit.address = (iptr) data;
   accelConfig->EXAMPLE_UnconnectedUnit2.unit.content = 123;
   accelConfig->EXAMPLE_UnconnectedUnit2.unit.length = 4;

   RunAccelerator(3);

   ClearCache(arena->mem);

   Assert_Eq(*data,123);

   accelConfig->EXAMPLE_UnconnectedUnit2.unit.content = 321;

   RunAccelerator(3);

   ClearCache(arena->mem);

   Assert_Eq(*data,321);
}