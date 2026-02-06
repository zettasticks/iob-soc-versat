#include "testbench.hpp"

void SingleTest(Arena* arena){
   int memory = 3;

   EXAMPLE_Variety1_Simple(&memory,1,1,5,4);

   VersatUnitWrite(TOP_m_addr,0,2);

   RunAccelerator(2);

   EXAMPLE_Variety1_Result_Struct res = EXAMPLE_Variety1_Result();
   Assert_Eq(res.res,15);
}
