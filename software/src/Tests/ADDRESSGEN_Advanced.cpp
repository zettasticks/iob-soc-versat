#include "testbench.hpp"

void SingleTest(Arena* arena){
   int buffer[1000] = {};

   int values = 8;
   ConfigureMemoryReceive(&accelConfig->output,values);

   buffer[0]  = 10;
   buffer[2]  = 20;
   buffer[4]  = 30;
   buffer[6]  = 40;
   buffer[5]  = 50;
   buffer[7]  = 60;
   buffer[9]  = 70;
   buffer[11] = 80;

   Configure(&accelConfig->read,(iptr) buffer,2,4,2,5);
   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
   }
   printf("\n");


   buffer[0]   = 10;
   buffer[2]   = 20;
   buffer[4]   = 30;
   buffer[6]   = 40;
   buffer[100] = 50;
   buffer[102] = 60;
   buffer[104] = 70;
   buffer[106] = 80;

   Configure(&accelConfig->read,(iptr) buffer,2,4,2,100);
   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
   }
   printf("\n");


   buffer[0]  = 10;
   buffer[5]  = 20;
   buffer[4] = 30;
   buffer[9] = 40;
   buffer[8]  = 50;
   buffer[13] = 60;
   buffer[12] = 70;
   buffer[18] = 80;

   Configure(&accelConfig->read,(iptr) buffer,4,2,5,4);
   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
   }
   printf("\n");


   buffer[0]  = 10;
   buffer[5]  = 20;
   buffer[10] = 30;
   buffer[15] = 40;
   buffer[2]  = 50;
   buffer[7]  = 60;
   buffer[12] = 70;
   buffer[17] = 80;

   Configure(&accelConfig->read,(iptr) buffer,2,4,5,2);
   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
   }
   printf("\n");



}