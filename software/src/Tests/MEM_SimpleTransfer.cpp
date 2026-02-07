#include "testbench.hpp"

void SingleTest(Arena* arena){
  int numberItems = 4;

  MEM_SimpleTransfer_Simple(numberItems);

  for(int i = 0; i < numberItems; i++){
    VersatUnitWrite(TOP_start_addr,i,i + 1);
  }

  RunAccelerator(3);
  
  for(int i = 0; i < numberItems; i++){
    int read = VersatUnitRead(TOP_end_addr,i);
    Assert_Eq(i + 1,read);
  }
}
