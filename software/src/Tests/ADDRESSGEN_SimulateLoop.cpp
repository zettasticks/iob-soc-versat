#include "testbench.hpp"

void SingleTest(Arena* arena){
   int input[] = {1,2,3,4,0,0,0,0,5,6,7,8,0,0,0,0,9,10,11,12,0,0,0,0,13,14,15,16};
   int expected[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

   iptr pointers[100];

   printResults = true;

   Assert_Eq(1,0);
   return;

#if 0
   if(0){
      // VRead Multiple Load + Linear
      auto args = CompileVUnit_VDouble(input,4,4,8);
      
      SimulateAndPrintAddressGen(args);

      int totalSamples = SimulateAddressGen(pointers,100,args);
      Assert_Eq(totalSamples,(int) ARRAY_SIZE(expected));

      for(int i = 0; i < totalSamples; i++){
         int* val = (int*) pointers[i];
      }

      for(int i = 0; i < ARRAY_SIZE(expected); i++){
         Assert_Eq(*((int*)pointers[i]),expected[i]);
      }
   }

   if(0){
      auto args = CompileVUnit_VDouble(input,0,0,0);
      int totalSamples = SimulateAddressGen(pointers,100,args);

      Assert_Eq(totalSamples,0);
   }

   if(0){
      auto args = CompileVUnit_VDouble(input,1,1,1);
      int totalSamples = SimulateAddressGen(pointers,100,args);

      Assert_Eq(totalSamples,1);
   }

   if(0){
      auto args = CompileVUnit_FullUsage(input,4,4,4,1,1,1,1,1);
      int totalSamples = SimulateAddressGen(pointers,100,args);
      Assert_Eq(totalSamples,16);

      for(int i = 0; i < totalSamples; i++){
         int* val = (int*) pointers[i];
         printf("%p\n",val);
      }
   }
#endif
}
