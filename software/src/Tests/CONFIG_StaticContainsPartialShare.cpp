#include "testbench.hpp"

void SingleTest(Arena* arena){
#if 0
   accelConfig->units_0.constant = 5;
   accelConfig->units_0.constant2 = 10;
   accelConfig->units_1.constant = 15;
   accelConfig->units_1.constant2 = 20;

   RunAccelerator(3);

   Assert_Eq(accelState->outputs_0.currentValue,5);
   Assert_Eq(accelState->outputs_1.currentValue,20);
   Assert_Eq(accelState->outputs_2.currentValue,15);
   Assert_Eq(accelState->outputs_3.currentValue,20);
#endif
}