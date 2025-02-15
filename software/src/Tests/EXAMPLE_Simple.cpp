#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->a.constant = 10;
   accelConfig->b.constant = 5;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_result_currentValue,15);
}
