#include "testbench.hpp"

void SingleTest(Arena* arena){
   // Any given combination of x and y should still add to the same value

   accelConfig->childs_0.x_0.constant = 1;
   accelConfig->childs_1.y_1.constant = 1;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_outputs_0_currentValue,2);
   Assert_Eq(accelState->TOP_outputs_1_currentValue,2);
   Assert_Eq(accelState->TOP_outputs_2_currentValue,2);
   Assert_Eq(accelState->TOP_outputs_3_currentValue,2);

   accelConfig->childs_0.x_1.constant = 2;
   accelConfig->childs_1.y_0.constant = 2;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_outputs_0_currentValue,4);
   Assert_Eq(accelState->TOP_outputs_1_currentValue,4);
   Assert_Eq(accelState->TOP_outputs_2_currentValue,4);
   Assert_Eq(accelState->TOP_outputs_3_currentValue,4);

   accelConfig->childs_1.x_1.constant = 3;
   accelConfig->childs_0.y_0.constant = 3;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_outputs_0_currentValue,6);
   Assert_Eq(accelState->TOP_outputs_1_currentValue,6);
   Assert_Eq(accelState->TOP_outputs_2_currentValue,6);
   Assert_Eq(accelState->TOP_outputs_3_currentValue,6);

   accelConfig->childs_1.x_0.constant = 4;
   accelConfig->childs_0.y_1.constant = 4;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_outputs_0_currentValue,8);
   Assert_Eq(accelState->TOP_outputs_1_currentValue,8);
   Assert_Eq(accelState->TOP_outputs_2_currentValue,8);
   Assert_Eq(accelState->TOP_outputs_3_currentValue,8);
}