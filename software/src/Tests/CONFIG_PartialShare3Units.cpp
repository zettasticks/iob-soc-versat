#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->units_0.constant = 5;
   accelConfig->units_0.constant2 = 10;
   accelConfig->units_1.constant = 15;
   accelConfig->units_1.constant2 = 20;
   accelConfig->units_2.constant = 25;
   accelConfig->units_2.constant2 = 30;

   RunAccelerator(3);

   Assert_Eq(accelState->outputs_0.currentValue,5);
   Assert_Eq(accelState->outputs_1.currentValue,30);
   Assert_Eq(accelState->outputs_2.currentValue,15);
   Assert_Eq(accelState->outputs_3.currentValue,30);
   Assert_Eq(accelState->outputs_4.currentValue,25);
   Assert_Eq(accelState->outputs_5.currentValue,30);
}