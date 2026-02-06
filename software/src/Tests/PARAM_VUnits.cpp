#include "testbench.hpp"

#define SIZE 1024

void SingleTest(Arena* arena){
   int* in = (int*) PushBytes(arena,sizeof(int) * SIZE);
   int* out = (int*) PushBytes(arena,sizeof(int) * SIZE);
   for(int i = 0; i < SIZE; i++){
      in[i] = i + 1;
   }

   ClearCache(arena->mem);

   VersatVarSpec width = {};
   width.min = 1;
   width.max = 1024;
   width.order = 0;

   int sizePerLoop = PARAM_VUnits_Init_Size(&width);
   printf("%d:%d\n",sizePerLoop,width.value);
   int maxTransfer = width.value;

   printf("%d\n",maxTransfer);

   for(int i = 0; i < SIZE; i += maxTransfer){
      int toTransfer = MIN(SIZE - i,maxTransfer);

      PARAM_VUnits_Init(in + i,out + i,toTransfer);
      RunAccelerator(1);
   }

   RunAccelerator(2);

   ClearCache(arena->mem);

   for(int i = 0; i < SIZE; i++){
      Assert_Eq(in[i],out[i]);
   }
}