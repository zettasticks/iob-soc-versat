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
   printf("Efficiency: %2f\n",percent);
}

void Test(int* buffer,int offset,int expected){
   AddressVArguments compiled = CompileVUnit_AddressGenConstant(buffer,offset);
   LoadVUnit_VRead(&accelConfig->read,compiled);
   CheckEfficiency(compiled);

   RunAccelerator(3);
   Assert_Eq(VersatUnitRead(TOP_output_addr,0),expected);
}

void SingleTest(Arena* arena){
   int buffer[1000] = {};

   int values = 1;
   ConfigureMemoryReceive(&accelConfig->output,values);

   ClearBuffer(buffer);

   buffer[5+1]    = 100;
   buffer[5+10]   = 200;
   buffer[5+100]  = 300;
   buffer[5+1000] = 400;

   Test(buffer,1   ,100);
   Test(buffer,10  ,200);
   Test(buffer,100 ,300);
   Test(buffer,1000,400);
}