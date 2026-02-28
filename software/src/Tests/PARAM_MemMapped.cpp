#include "testbench.hpp"

void SingleTest(Arena* arena){
   int data[4] = {0,321,0,0};

   PARAM_MemMapped_Load(&data,4);
   PARAM_MemMapped_Simple(1);
 
   RunAccelerator(3);

   PARAM_MemMapped_Result_Struct res = PARAM_MemMapped_Result();
   Assert_Eq(res.res,321);
}
