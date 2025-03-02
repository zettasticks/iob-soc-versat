#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->consts_0.constant = 1;
   accelConfig->consts_0.constant2 = 2;
   accelConfig->consts_1.constant = 3;
   accelConfig->consts_1.constant2 = 4;

   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts2_constant = 10;
   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts2_constant2 = 20;

   accelConfig->consts3_0.constant = 30;
   accelConfig->consts3_0.constant2 = 35;
   accelConfig->consts3_1.constant = 40;
   accelConfig->consts3_1.constant2 = 45;

   // If working correctly, this should override the first configs
   accelConfig->consts3_0.constant = 100;
   accelConfig->consts3_1.constant = 200;
   accelConfig->consts3_1.constant2 = 150;

   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts4_constant = 210;
   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts4_constant2 = 220;

   RunAccelerator(3);

   Assert_Eq(accelState->outputs_0.currentValue,3);
   Assert_Eq(accelState->outputs_1.currentValue,4);
   Assert_Eq(accelState->outputs_2.currentValue,3);
   Assert_Eq(accelState->outputs_3.currentValue,4);

   Assert_Eq(accelState->outputs_4.currentValue,10);
   Assert_Eq(accelState->outputs_5.currentValue,20);

   Assert_Eq(accelState->outputs_6.currentValue,100);
   Assert_Eq(accelState->outputs_7.currentValue,150);
   Assert_Eq(accelState->outputs_8.currentValue,200);
   Assert_Eq(accelState->outputs_9.currentValue,150);

   Assert_Eq(accelState->outputs_10.currentValue,210);
   Assert_Eq(accelState->outputs_11.currentValue,220);
}