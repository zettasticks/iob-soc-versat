#include "testbench.hpp"

void SingleTest(Arena* arena){
   iptr addrA = 0;
   iptr addrB = 1;

   iptr indexA = 0;
   iptr indexB = 4;

   VersatUnitWrite(TOP_table_addr,addrA,0xf0);
   VersatUnitWrite(TOP_table_addr,addrB,0xf4);

   accelConfig->input_0.constant = indexA;
   accelConfig->input_1.constant = indexB;
   
   RunAccelerator(3);
  
   Assert_Eq(accelState->output_0.currentValue,0xf0);
   Assert_Eq(accelState->output_1.currentValue,0xf4);
}
