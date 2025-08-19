#include "testbench.hpp"

void SingleTest(Arena* arena){
   int* buffer = (int*) PushBytes(arena,sizeof(int) * 100); 
   //            543210
   int tests = 0b111111;

   // Generator and memory units
   if(tests & (1 << 0)){
      ResetAccelerator();
      
      forceSingleLoop = false;
      forceDoubleLoop = true;
      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      StridedLinear_Mem_Input_0(&accelConfig->mem,4,4);

      RunAccelerator(3);

      int results[4] = {};
      for(int i = 0; i < 4; i++){
         results[i] = VersatUnitRead(TOP_mem_addr,i);
      }

      int expected[4] = {3,3,3,3};
      Assert_Eq(results,expected,4,"0");
   }

   if(tests & (1 << 1)){
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
      Assert_Eq(results,expected,4,"1");
   }

   // Test VWrite
   if(tests & (1 << 2)){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      forceSingleLoop = false;
      forceDoubleLoop = true;

      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      StridedLinear_VWrite(&accelConfig->write,buffer,4,4);

      RunAccelerator(3);
      ClearCache(buffer);

      int expected[4] = {3,3,3,3};
      Assert_Eq(buffer,expected,4,"2");
   }

   if(tests & (1 << 3)){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      forceSingleLoop = true;
      forceDoubleLoop = false;

      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      StridedLinear_VWrite(&accelConfig->write,buffer,4,4);

      RunAccelerator(3);
      ClearCache(buffer);

      int expected[4] = {3,3,3,3};
      Assert_Eq(buffer,expected,4,"3");
   }

   if(tests & (1 << 4)){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      int expected[8] = {3,3,0,0,
                         3,3,0,0};

      forceSingleLoop = true;
      forceDoubleLoop = false;

      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      Strided2_VWrite(&accelConfig->write,buffer,2,2,4,4);

      RunAccelerator(3);
      ClearCache(buffer);

      Assert_Eq(buffer,expected,8,"DoubleStride - SingleLoop");
   }

   if(tests & (1 << 5)){
      ResetAccelerator();
      ClearBuffer(buffer,100);

      int expected[8] = {3,3,0,0,
                         3,3,0,0};

      forceSingleLoop = false;
      forceDoubleLoop = true;

      TestAddressGen_Generator(&accelConfig->gen,2,2,2,2,2);
      Strided2_VWrite(&accelConfig->write,buffer,2,2,4,4);

      RunAccelerator(3);
      ClearCache(buffer);

      Assert_Eq(buffer,expected,8,"DoubleStride - DoubleLoop");
   }
}