
#include "testbench.hpp"

#define MAX_SIZE 1000
void ClearBuffer(int* buffer){
   for(int i = 0; i < MAX_SIZE; i++){
      buffer[i] = 0;
   }
}

void SingleTest(Arena* arena){
   int expectedValues[] = {1,2,3,4};
   int buffer[1000] = {};

   int values = 4;
   ConfigureMemoryReceive(&accelConfig->output,values);

   ClearBuffer(buffer);
   buffer[0]  = 1;
   buffer[1]  = 2;
   buffer[2]  = 3;
   buffer[3]  = 4;

   {
      AddressVArguments args = CompileVUnit_AddressGenZeroValues(buffer,2,2,0);
      LoadVUnit_VRead(&accelConfig->read,args);
      PrintArguments(args);

      RunAccelerator(3);
      for(int i = 0; i < values; i++){
         printf("%d\n",VersatUnitRead(TOP_output_addr,i));
         Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues[i]);
      }
      printf("\n");
   }

}