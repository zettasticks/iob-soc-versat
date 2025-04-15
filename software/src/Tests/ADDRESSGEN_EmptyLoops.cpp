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
   int expectedValues[] = {10,10,20,20,30,30,40,40};
   int expectedValues2[] = {10,20,10,20,30,40,30,40};
   int expectedValues3[] = {10,20,30,40,10,20,30,40};
   int expectedValues4[] = {10,10,10,10,10,20,20,20,20,20,30,30,30,30,30,40,40,40,40,40};
   int buffer[1000] = {};

   int values = 8;
   ConfigureMemoryReceive(&accelConfig->output,values);

   ClearBuffer(buffer);
   buffer[0]  = 10;
   buffer[2]  = 20;
   buffer[5]  = 30;
   buffer[7]  = 40;

{
   AddressVArguments compiled = CompileVUnit_AddressGenEmptyLoops(buffer);
   LoadVUnit_VRead(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues[i]);
   }
   printf("\n");
}

{
   AddressVArguments compiled = CompileVUnit_AddressGenEmptyLoops2(buffer);
   LoadVUnit_VRead(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues2[i]);
   }
   printf("\n");
}

{
   AddressVArguments compiled = CompileVUnit_AddressGenEmptyLoops3(buffer);
   LoadVUnit_VRead(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues3[i]);
   }
   printf("\n");
}
   
   values = 20;
   ClearBuffer(buffer);
   buffer[0]  = 10;
   buffer[1]  = 20;
   buffer[2]  = 30;
   buffer[3]  = 40;
   ConfigureMemoryReceive(&accelConfig->output,values);

{
   AddressVArguments compiled = CompileVUnit_AddressGenEmptyLoops4(buffer);
   LoadVUnit_VRead(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   for(int i = 0; i < values; i++){
      printf("%d\n",VersatUnitRead(TOP_output_addr,i));
      Assert_Eq(VersatUnitRead(TOP_output_addr,i),expectedValues4[i]);
   }
   printf("\n");
}
}