#include "testbench.hpp"

void SingleTest(Arena* arena){
   RunAccelerator(1);
   PARAM_Simple_Result_Struct res = PARAM_Simple_Result();
   Assert_Eq(res.res,123);
}