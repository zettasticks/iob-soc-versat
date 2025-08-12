#include "testbench.hpp"

void SingleTest(Arena* arena){
   int* buffer = (int*) PushBytes(arena,sizeof(int) * 100); 

   // Generator and memory units
   // 0
   if(0){
      ResetAccelerator();
      
      forceSingleLoop = false;
      forceDoubleLoop = true;
      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      StridedLinear_Mem_Input_0(&accelConfig->mem,4,4);

      int results[4] = {};
      for(int i = 0; i < 4; i++){
         results[i] = VersatUnitRead(TOP_mem_addr,i);
      }

      int expected[4] = {3,3,3,3};
      Assert_Eq(results,expected,4);
   }

   // 1
   if(0){
      ResetAccelerator();

      forceSingleLoop = true;
      forceDoubleLoop = false;
      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      StridedLinear_Mem_Input_0(&accelConfig->mem,4,4);

      RunAccelerator(3);

      int results[4] = {};
      for(int i = 0; i < 4; i++){
         results[i] = VersatUnitRead(TOP_mem_addr,i);
      }

      int expected[4] = {3,3,3,3};
      Assert_Eq(results,expected,4);
   }

   // Test VWrite
   // 2
   if(0){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      forceSingleLoop = false;
      forceDoubleLoop = true;

      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      StridedLinear_VWrite(&accelConfig->write,buffer,4,4);

      RunAccelerator(3);
      ClearCache(buffer);

      int expected[4] = {3,3,3,3};
      Assert_Eq(buffer,expected,4);
   }

   // 3
   if(0){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      forceSingleLoop = true;
      forceDoubleLoop = false;

      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      StridedLinear_VWrite(&accelConfig->write,buffer,4,4);

      RunAccelerator(3);
      ClearCache(buffer);

      int expected[4] = {3,3,3,3};
      Assert_Eq(buffer,expected,4);
   }

   // 4
   if(1){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      int expected[8] = {3,3,0,0,
                         3,3,0,0};

      forceSingleLoop = true;
      forceDoubleLoop = false;

      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      Strided2_VWrite(&accelConfig->write,buffer,2,2,4,4);
      //Compared_VWrite(&accelConfig->write,buffer,2,2,4);

      RunAccelerator(3);
      ClearCache(buffer);

      Assert_Eq(buffer,expected,8);
   }

   // 5
   if(0){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      int expected[8] = {3,3,0,0,
                         3,3,0,0};

      forceSingleLoop = false;
      forceDoubleLoop = true;

      Strided2_VWrite(&accelConfig->write,buffer,2,2,4,4);

      RunAccelerator(3);
      ClearCache(buffer);

      Assert_Eq(buffer,expected,8);
   }
}