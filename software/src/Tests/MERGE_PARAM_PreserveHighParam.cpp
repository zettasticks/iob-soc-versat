#include "testbench.hpp"

void SingleTest(Arena* arena){
   int size = 64;

   int* input = (int*) PushBytes(arena,sizeof(int) * 128);
   int* output = (int*) PushBytes(arena,sizeof(int) * 128);
   int* output2 = (int*) PushBytes(arena,sizeof(int) * 128);

   for(int i = 0; i < size; i++){
      input[i] = i + 1;
   }

   ActivateMergedAccelerator(MergeType_MERGE_PARAM_Temp1);
   MERGE_PARAM_Temp1_Simple(input,output,size);
  
   RunAccelerator(1);

   ActivateMergedAccelerator(MergeType_MERGE_PARAM_Temp2);
   MERGE_PARAM_Temp2_Simple(input,output2,size);

   RunAccelerator(3);

   for(int i = 0; i < size; i++){
      Assert_Eq(input[i],output[i]);
      Assert_Eq(input[i],output2[i]);
   }
}