#include "testbench.hpp"

#define MAX_SIZE 1000
void ClearBuffer(int* buffer){
   for(int i = 0; i < MAX_SIZE; i++){
      buffer[i] = 0;
   }
}

void SingleTest(Arena* arena){
   int expectedValues[] = {10,20,30,40,50,60,70,80};
   int buffer[MAX_SIZE] = {};
   int values = 8;

   ConfigureMemoryReceive(&accelConfig->ADDRESSGEN_Advanced.output,values);

   {
      ClearBuffer(buffer);
      buffer[0]  = 10;
      buffer[2]  = 20;
      buffer[4]  = 30;
      buffer[6]  = 40;
      buffer[5]  = 50;
      buffer[7]  = 60;
      buffer[9]  = 70;
      buffer[11] = 80;

      AddressGenAdvancedTest_VRead(&accelConfig->ADDRESSGEN_Advanced.read,buffer,2,4,2,5);  

      RunAccelerator(3);
      for(int i = 0; i < values; i++){
         printf("%d\n",VersatUnitRead(TOP_output_output_addr,i));
         Assert_Eq(VersatUnitRead(TOP_output_output_addr,i),expectedValues[i]);
      }
      printf("\n");      
   }
}