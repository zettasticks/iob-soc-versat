#include "testbench.hpp"

void SingleTest(Arena* arena){
   ActivateMergedAccelerator(MergeType_PARAM_Simple);
   RunAccelerator(3);

   PARAM_Simple_Result_Struct s1 = PARAM_Simple_Result();

   Assert_Eq(s1.res,123);

   ActivateMergedAccelerator(MergeType_PARAM_Temp1);
   RunAccelerator(3);

   PARAM_Temp1_Result_Struct s2 = PARAM_Temp1_Result();
   Assert_Eq(s2.res,321);
}