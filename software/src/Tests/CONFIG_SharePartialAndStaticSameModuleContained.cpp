#include "testbench.hpp"

void SingleTest(Arena* arena){
   accelConfig->container.consts_0.constant = 1;
   accelConfig->container.consts_0.constant2 = 2;
   accelConfig->container.consts_1.constant = 3;
   accelConfig->container.consts_1.constant2 = 4;

   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts2_constant = 10;
   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts2_constant2 = 20;

   accelConfig->container.consts3_0.constant = 30;
   accelConfig->container.consts3_0.constant2 = 35;
   accelConfig->container.consts3_1.constant = 40;
   accelConfig->container.consts3_1.constant2 = 45;

   // If working correctly, this should override the first configs
   accelConfig->container.consts3_0.constant = 100;
   accelConfig->container.consts3_1.constant = 200;
   accelConfig->container.consts3_1.constant2 = 150;

   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts4_constant = 210;
   accelStatic->CONFIG_SharePartialAndStaticSameModule_consts4_constant2 = 220;

   RunAccelerator(3);

   Assert_Eq(accelState->TOP_container_outputs_0_currentValue,3);
   Assert_Eq(accelState->TOP_container_outputs_1_currentValue,4);
   Assert_Eq(accelState->TOP_container_outputs_2_currentValue,3);
   Assert_Eq(accelState->TOP_container_outputs_3_currentValue,4);

   Assert_Eq(accelState->TOP_container_outputs_4_currentValue,10);
   Assert_Eq(accelState->TOP_container_outputs_5_currentValue,20);

   Assert_Eq(accelState->TOP_container_outputs_6_currentValue,100);
   Assert_Eq(accelState->TOP_container_outputs_7_currentValue,150);
   Assert_Eq(accelState->TOP_container_outputs_8_currentValue,200);
   Assert_Eq(accelState->TOP_container_outputs_9_currentValue,150);

   Assert_Eq(accelState->TOP_container_outputs_10_currentValue,210);
   Assert_Eq(accelState->TOP_container_outputs_11_currentValue,220);
}