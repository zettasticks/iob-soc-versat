#include "testbench.hpp"

void SingleTest(Arena* arena){
   MERGE_DelaysAddr addr = ACCELERATOR_TOP_ADDR_INIT;

   int data[] = {0,1,2,3,4,5,6,7,8,9};

   int dataSize = ARRAY_SIZE(data);

   for(int i = 0; i < ARRAY_SIZE(data); i++){
      VersatUnitWrite(addr.m.addr,i,data[i]);
   }

   // Because the unit is shared, no need to do this, although it would be best in a real usage of the accelerator

   ActivateMergedAccelerator(MergeType_TestMergeDelay1);
   TestMergeDelay1_Linear(ARRAY_SIZE(data));

   RunAccelerator(3);
   Assert_Eq(accelState->TestMergeDelay1.output.currentValue,3);   

   ActivateMergedAccelerator(MergeType_TestMergeDelay2);
   TestMergeDelay2_Linear(ARRAY_SIZE(data));

   RunAccelerator(3);
   Assert_Eq(accelState->TestMergeDelay2.output.currentValue,5);
}