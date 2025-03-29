#include "testbench.hpp"

void SingleTest(Arena* arena){
   int buffer[300] = {};

   buffer[0] = 10;
   buffer[2] = 20;
   buffer[5] = 30;
   buffer[7] = 40;

   buffer[11] = 50;
   buffer[13] = 60;
   buffer[16] = 70;
   buffer[18] = 80;

   buffer[20+0] = 110;
   buffer[20+2] = 120;
   buffer[20+5] = 130;
   buffer[20+7] = 140;

   buffer[20+11] = 150;
   buffer[20+13] = 160;
   buffer[20+16] = 170;
   buffer[20+18] = 180;

   buffer[100+0] = 210;
   buffer[100+2] = 220;
   buffer[100+5] = 230;
   buffer[100+7] = 240;

   buffer[100+11] = 250;
   buffer[100+13] = 260;
   buffer[100+16] = 270;
   buffer[100+18] = 280;

   buffer[100+20+0] = 2110;
   buffer[100+20+2] = 2120;
   buffer[100+20+5] = 2130;
   buffer[100+20+7] = 2140;

   buffer[100+20+11] = 2150;
   buffer[100+20+13] = 2160;
   buffer[100+20+16] = 2170;
   buffer[100+20+18] = 2180;

   int values = 32;

   Configure(&accelConfig->read,(iptr) buffer);
   ConfigureMemoryReceive(&accelConfig->output,values);

   RunAccelerator(3);

   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
   }
}