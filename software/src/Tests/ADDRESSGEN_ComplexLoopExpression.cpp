#include "testbench.hpp"

#define MAX_SIZE 10
#define VALUES 4

void SingleTest(Arena* arena){
   int expectedValues[] = {10,20,30,40};
   int* buffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);

   if(1){
      ResetAccelerator();

      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[1]  = 20;
      buffer[2]  = 30;
      buffer[3]  = 40;

      VLinear_Mem_Input_0(&accelConfig->output,VALUES);
      AddressGenComplexLoopExpression_VRead(&accelConfig->read,buffer,16,4);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[VALUES];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"1");
   }

}