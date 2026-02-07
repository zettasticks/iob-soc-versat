#include "testbench.hpp"

#define MAX_SIZE 4

void SingleTest(Arena* arena){
   int data[] = {1,2,3,4};

   VersatMemoryCopy(TOP_start_addr,data,sizeof(int) * 4);

   ADDRESSGEN_Mem_Simple(4);

#if 0
   VLinear_Mem_Output_0(&accelConfig->start,4);
   VLinear_Mem_Input_0(&accelConfig->end,4);
#endif
   
   RunAccelerator(3);

   int result[MAX_SIZE];
   for(int i = 0; i < MAX_SIZE; i++){
      result[i] = VersatUnitRead(TOP_end_addr,i);
   }   

   Assert_Eq(result,data,MAX_SIZE);
}