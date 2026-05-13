#include "testbench.hpp"

void SingleTest(Arena* arena){
   ActivateMergedAccelerator(MergeType_MERGE_BufferTemp1);
   MERGE_BufferTemp1_Init();
   RunAccelerator(3);
   Assert_Eq(MERGE_BufferTemp1_Read().res,8);

   ActivateMergedAccelerator(MergeType_MERGE_BufferTemp2);
   MERGE_BufferTemp2_Init();
   RunAccelerator(3);
   Assert_Eq(MERGE_BufferTemp2_Read().res,8);
}