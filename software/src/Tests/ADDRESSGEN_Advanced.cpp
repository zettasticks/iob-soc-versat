#include "testbench.hpp"

void SingleTest(Arena* arena){
   int buffer[300] = {};

   buffer[0] = 10;
   buffer[5] = 20;
   buffer[100] = 30;
   buffer[105] = 40;

   Configure(&accelConfig->read,(iptr) buffer);
   ConfigureMemoryReceive(&accelConfig->output,4);

   RunAccelerator(3);

   printf("%d\n",VersatUnitRead(TOP_output_addr,0));
   printf("%d\n",VersatUnitRead(TOP_output_addr,1));
   printf("%d\n",VersatUnitRead(TOP_output_addr,2));
   printf("%d\n",VersatUnitRead(TOP_output_addr,3));
}