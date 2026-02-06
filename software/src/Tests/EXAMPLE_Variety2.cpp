#include "testbench.hpp"

void SingleTest(Arena* arena){
   int memory = 2;
   int* output = (int*) PushBytes(arena,sizeof(int));
 
   EXAMPLE_Variety2_Simple(&memory,output,1,5,4,3);
   VersatUnitWrite(TOP_m_addr,0,1);

   RunAccelerator(3);

   Assert_Eq(*output,15);
}