#include "testbench.hpp"

#define MAX_SIZE 1000
void ClearBuffer(int* buffer){
   for(int i = 0; i < MAX_SIZE; i++){
      buffer[i] = 0;
   }
}

void CheckEfficiency(AddressVArguments args){
   SimulateVReadResult sim = SimulateVRead(args);
   float percent = ((float) sim.amountOfInternalValuesUsed) / ((float) sim.amountOfExternalValuesRead);
   printf("Efficiency: %2f (%d/%d)\n",percent,sim.amountOfInternalValuesUsed,sim.amountOfExternalValuesRead);
}

void SingleTest(Arena* arena){
   int expectedValues[] = {10,20,30,40};
   int buffer[1000] = {};

   int values = 4;
   ConfigureMemoryReceive(&accelConfig->output,values);

   ClearBuffer(buffer);
   buffer[0]  = 10;
   buffer[1]  = 20;
   buffer[2]  = 30;
   buffer[3]  = 40;

{
   AddressVArguments compiled = CompileVUnit_AddressGenComplexLoopExpression(buffer,16,4);
   LoadVUnit_VRead(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues[i]);
   }
   printf("\n");
}

}