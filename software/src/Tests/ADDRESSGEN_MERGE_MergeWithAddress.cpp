#include "testbench.hpp"

#define MAX_SIZE 20
#define VALUES 8

void SingleTest(Arena* arena){
   int expectedValues[] = {10,20,30,40,50,60,70,80};
   int* buffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);

   // TODO: Missing exercising of the ADDRESSGEN_ComplexLoopExpression module

   if(1){
      ResetAccelerator();

      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[2]  = 20;
      buffer[4]  = 30;
      buffer[6]  = 40;
      buffer[5]  = 50;
      buffer[7]  = 60;
      buffer[9]  = 70;
      buffer[11] = 80;

      ADDRESSGEN_Advanced_Read(buffer,2,4,2,5);
      ADDRESSGEN_Advanced_Mem(VALUES);

      //VLinear_Mem_Input_0(&accelConfig->ADDRESSGEN_Advanced.output,VALUES);
      //AddressGenAdvancedTest_VRead(&accelConfig->ADDRESSGEN_Advanced.read,buffer,2,4,2,5);  

      RunAccelerator(3);

      int result[MAX_SIZE];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"1");
   }
}