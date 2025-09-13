#include "testbench.hpp"

void SingleTest(Arena* arena){
  int numberItems = 4;

  VLinear_Mem_Output_0(&accelConfig->start,numberItems);
  VLinear_Mem_Input_0(&accelConfig->end,numberItems);

  for(int i = 0; i < numberItems; i++){
    VersatUnitWrite(TOP_start_addr,i,i + 1);
  }

  RunAccelerator(3);
  
  for(int i = 0; i < numberItems; i++){
    int read = VersatUnitRead(TOP_end_addr,i);
    Assert_Eq(i + 1,read);
  }
}
