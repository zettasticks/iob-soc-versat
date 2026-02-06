#include "testbench.hpp"

void SingleTest(Arena* arena){
   MERGE_TwoLevels_SimpleConfig* config = (MERGE_TwoLevels_SimpleConfig*) accelConfig;
   MERGE_TwoLevels_SimpleAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   int x[] = {7};
   int y[] = {20};

   {
      ActivateMergedAccelerator(MergeType_TestDoubleMerge0_TestDoubleMerge00);

      TestDoubleMerge00_Simple(10,5);

      //config->simple.TestDoubleMerge0_TestDoubleMerge00.x00.constant = 10;
      //config->simple.TestDoubleMerge0_TestDoubleMerge00.y00.constant = 5;

      RunAccelerator(3);

      Assert_Eq(accelState->output_0.currentValue,15,"1");
   }

   {
      ActivateMergedAccelerator(MergeType_TestDoubleMerge0_TestDoubleMerge01);

      VersatUnitWrite(addr.simple.x01_x10_y11.addr,0,x[0]);
      VersatUnitWrite(addr.simple.y01.addr,0,y[0]);

      TestDoubleMerge01_Simple(1);

      //VLinear_Mem_Output_0(&config->simple.TestDoubleMerge0_TestDoubleMerge01.x01,1);
      //VLinear_Mem_Output_0(&config->simple.TestDoubleMerge0_TestDoubleMerge01.y01,1);

      RunAccelerator(3);

      Assert_Eq(accelState->output_0.currentValue,27,"2");
   }

   {
      ActivateMergedAccelerator(MergeType_TestDoubleMerge1_TestDoubleMerge10);

      //config->simple.TestDoubleMerge1_TestDoubleMerge10.y10.constant = 5;

      VersatUnitWrite(addr.simple.x01_x10_y11.addr,0,x[0]);
      //VLinear_Mem_Output_0(&config->simple.TestDoubleMerge1_TestDoubleMerge10.x10,1);

      TestDoubleMerge10_Simple(1,5);

      RunAccelerator(3);

      Assert_Eq(accelState->output_0.currentValue,-2,"3");
   }

   {
      ActivateMergedAccelerator(MergeType_TestDoubleMerge1_TestDoubleMerge11);

      VersatUnitWrite(addr.simple.x01_x10_y11.addr,0,y[0]);
      //VLinear_Mem_Output_0(&config->simple.TestDoubleMerge1_TestDoubleMerge11.y11,1);
      //config->simple.TestDoubleMerge1_TestDoubleMerge11.x11.constant = 2;

      TestDoubleMerge11_Simple(1,2);

      RunAccelerator(3);

      Assert_Eq(accelState->output_0.currentValue,18,"4");
   }
}