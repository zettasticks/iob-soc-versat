#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->testShare_0.inside.constant = 1;
   accelConfig->testShare_1.inside.constant = 2; // Since testShare_0 and 1 share same config, this should override the 1 from testShare_0

   ACCEL_TestStaticChild1_child_inside0_constant = 3;
   ACCEL_TestStaticChild2_inside1_constant = 4;
   
   ACCEL_TestShareStaticChild_shareChild_x_0_constant = 5;
   ACCEL_TestShareStaticChild_shareChild_y_0_constant = 6;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_outputs_0_currentValue,3);
   Assert_Eq(accelState->TOP_outputs_1_currentValue,4);
   Assert_Eq(accelState->TOP_outputs_2_currentValue,2);
   Assert_Eq(accelState->TOP_outputs_3_currentValue,3);
   Assert_Eq(accelState->TOP_outputs_4_currentValue,4);
   Assert_Eq(accelState->TOP_outputs_5_currentValue,2);

   Assert_Eq(accelState->TOP_outputs_6_currentValue,11);
   Assert_Eq(accelState->TOP_outputs_7_currentValue,11);
   Assert_Eq(accelState->TOP_outputs_8_currentValue,11);
   Assert_Eq(accelState->TOP_outputs_9_currentValue,11);
}