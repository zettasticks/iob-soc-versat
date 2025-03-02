#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->shares_0.units_0.constant = 5;
   accelConfig->shares_0.units_0.constant2 = 10;
   accelConfig->shares_0.units_1.constant = 15;
   accelConfig->shares_0.units_1.constant2 = 20;

   // If working correctly, this should override the first configs
   accelConfig->shares_1.units_0.constant = 100;
   accelConfig->shares_1.units_1.constant = 200;
   accelConfig->shares_1.units_1.constant2 = 150;

   RunAccelerator(3);

   Assert_Eq(accelState->shares_0.outputs_0.currentValue,100);
   Assert_Eq(accelState->shares_0.outputs_1.currentValue,150);
   Assert_Eq(accelState->shares_0.outputs_2.currentValue,200);
   Assert_Eq(accelState->shares_0.outputs_3.currentValue,150);

   Assert_Eq(accelState->shares_1.outputs_0.currentValue,100);
   Assert_Eq(accelState->shares_1.outputs_1.currentValue,150);
   Assert_Eq(accelState->shares_1.outputs_2.currentValue,200);
   Assert_Eq(accelState->shares_1.outputs_3.currentValue,150);
}