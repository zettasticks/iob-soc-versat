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

   Assert_Eq(accelState->TOP_shares_0_outputs_0_currentValue,100);
   Assert_Eq(accelState->TOP_shares_0_outputs_1_currentValue,150);
   Assert_Eq(accelState->TOP_shares_0_outputs_2_currentValue,200);
   Assert_Eq(accelState->TOP_shares_0_outputs_3_currentValue,150);

   Assert_Eq(accelState->TOP_shares_1_outputs_0_currentValue,100);
   Assert_Eq(accelState->TOP_shares_1_outputs_1_currentValue,150);
   Assert_Eq(accelState->TOP_shares_1_outputs_2_currentValue,200);
   Assert_Eq(accelState->TOP_shares_1_outputs_3_currentValue,150);
}