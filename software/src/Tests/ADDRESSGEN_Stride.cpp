#include "testbench.hpp"

void SingleTest(Arena* arena){
   TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
   StridedLinear_Mem_Input_0(&accelConfig->mem,4,4); // Only store elements 3,7,11,15

   RunAccelerator(3);

   Assert_Eq(VersatUnitRead(TOP_mem_addr,0),3);
   Assert_Eq(VersatUnitRead(TOP_mem_addr,1),3);
   Assert_Eq(VersatUnitRead(TOP_mem_addr,2),3);
   Assert_Eq(VersatUnitRead(TOP_mem_addr,3),3);
}