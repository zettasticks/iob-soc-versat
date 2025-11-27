#include "testbench.hpp"

void SingleTest(Arena* arena){
   int* data = (int*) PushBytes(arena,8);

   accelConfig->unit.address = (iptr) data;
   accelConfig->unit.content = 123;
   accelConfig->unit.length = 4;

   RunAccelerator(3);

   ClearCache(arena->mem);

   Assert_Eq(*data,123);

   accelConfig->unit.content = 321;

   RunAccelerator(3);

   ClearCache(arena->mem);

   Assert_Eq(*data,321);
}