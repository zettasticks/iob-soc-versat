#include "testbench.hpp"

void SingleTest(Arena* arena){
   int data[] = {1,2,3,4};

   VersatMemoryCopy(TOP_start_addr,data,sizeof(int) * 4);

   VLinear_Mem_Output_0(&accelConfig->start,4);
   VLinear_Mem_Input_0(&accelConfig->end,4);
   
   RunAccelerator(3);

   for(int i = 0; i < 4; i++){
      Assert_Eq(data[i],VersatUnitRead(TOP_end_addr,i));
   }
}