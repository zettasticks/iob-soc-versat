#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->units_0.constant = 5;
   accelConfig->units_0.constant2 = 10;
   accelConfig->units_1.constant = 15;
   accelConfig->units_1.constant2 = 20;
   accelConfig->units_2.constant = 25;
   accelConfig->units_2.constant2 = 30;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_outputs_0_currentValue,5);
   Assert_Eq(accelState->TOP_outputs_1_currentValue,30);
   Assert_Eq(accelState->TOP_outputs_2_currentValue,15);
   Assert_Eq(accelState->TOP_outputs_3_currentValue,30);
   Assert_Eq(accelState->TOP_outputs_4_currentValue,25);
   Assert_Eq(accelState->TOP_outputs_5_currentValue,30);
}