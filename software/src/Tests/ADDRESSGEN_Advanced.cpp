#include "testbench.hpp"

#define MAX_SIZE 1000
void ClearBuffer(int* buffer){
   for(int i = 0; i < MAX_SIZE; i++){
      buffer[i] = 0;
   }
}

void CheckEfficiency(AddressVReadArguments args){
   SimulateVReadResult sim = SimulateVRead(args);
   float percent = ((float) sim.amountOfInternalValuesUsed) / ((float) sim.amountOfExternalValuesRead);
   printf("Efficiency: %2f\n",percent);
}

void SingleTest(Arena* arena){
   int expectedValues[] = {10,20,30,40,50,60,70,80};
   int buffer[1000] = {};

   int values = 8;
   ConfigureMemoryReceive(&accelConfig->output,values);

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

   AddressVReadArguments compiled = CompileVUnit_AddressGenAdvancedTest((iptr) buffer,2,4,2,5);
   LoadVUnit_VReadMultiple(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues[i]);
   }
   printf("\n");
}

{
   ClearBuffer(buffer);
   buffer[0]   = 10;
   buffer[2]   = 20;
   buffer[4]   = 30;
   buffer[6]   = 40;
   buffer[100] = 50;
   buffer[102] = 60;
   buffer[104] = 70;
   buffer[106] = 80;

   AddressVReadArguments compiled = CompileVUnit_AddressGenAdvancedTest((iptr) buffer,2,4,2,100);
   LoadVUnit_VReadMultiple(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues[i]);
   }
   printf("\n");
}

{
   ClearBuffer(buffer);
   buffer[0]  = 10;
   buffer[5]  = 20;
   buffer[4]  = 30;
   buffer[9]  = 40;
   buffer[8]  = 50;
   buffer[13] = 60;
   buffer[12] = 70;
   buffer[17] = 80;

   AddressVReadArguments compiled = CompileVUnit_AddressGenAdvancedTest((iptr) buffer,4,2,5,4);
   LoadVUnit_VReadMultiple(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues[i]);
   }
   printf("\n");
}

{
   ClearBuffer(buffer);
   buffer[0]  = 10;
   buffer[5]  = 20;
   buffer[10] = 30;
   buffer[15] = 40;
   buffer[2]  = 50;
   buffer[7]  = 60;
   buffer[12] = 70;
   buffer[17] = 80;

   AddressVReadArguments compiled = CompileVUnit_AddressGenAdvancedTest((iptr) buffer,2,4,5,2);
   LoadVUnit_VReadMultiple(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues[i]);
   }
   printf("\n");
}

}