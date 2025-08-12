#include "testbench.hpp"

#define MAX_SIZE 1000
#define VALUES 8

void SingleTest(Arena* arena){
   int expectedValues[] = {10,20,30,40,50,60,70,80};
   int* buffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);

   if(1){
      //ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      buffer[0]  = 10;
      buffer[2]  = 20;
      buffer[4]  = 30;
      buffer[6]  = 40;
      buffer[5]  = 50;
      buffer[7]  = 60;
      buffer[9]  = 70;
      buffer[11] = 80;

      AddressGenAdvancedTest_VRead(&accelConfig->read,buffer,2,4,2,5);
      VLinear_Mem_Input_0(&accelConfig->output,VALUES);

      ClearCache(buffer);
      RunAccelerator(3);
      
      int result[VALUES];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"1");
   }

   if(1){
      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]   = 10;
      buffer[2]   = 20;
      buffer[4]   = 30;
      buffer[6]   = 40;
      buffer[100] = 50;
      buffer[102] = 60;
      buffer[104] = 70;
      buffer[106] = 80;

      AddressGenAdvancedTest_VRead(&accelConfig->read,buffer,2,4,2,100);
      VLinear_Mem_Input_0(&accelConfig->output,VALUES);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[VALUES];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"2");
   }

   if(1){
      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[5]  = 20;
      buffer[4]  = 30;
      buffer[9]  = 40;
      buffer[8]  = 50;
      buffer[13] = 60;
      buffer[12] = 70;
      buffer[17] = 80;

      AddressGenAdvancedTest_VRead(&accelConfig->read,buffer,4,2,5,4);
      VLinear_Mem_Input_0(&accelConfig->output,VALUES);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[VALUES];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"3");
   }

   if(1){
      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[5]  = 20;
      buffer[10] = 30;
      buffer[15] = 40;
      buffer[2]  = 50;
      buffer[7]  = 60;
      buffer[12] = 70;
      buffer[17] = 80;

      AddressGenAdvancedTest_VRead(&accelConfig->read,buffer,2,4,5,2);
      VLinear_Mem_Input_0(&accelConfig->output,VALUES);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[VALUES];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"4");
   }
}