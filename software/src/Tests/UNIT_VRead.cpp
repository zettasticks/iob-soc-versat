#include "testbench.hpp"

void SingleTest(Arena* arena){
   int defaultData[] = {1,2,3,4};
   int* toLoad = (int*) PushBytes(arena,5 * sizeof(int));
   for(int i = 0; i < 4; i++){
      toLoad[i] = 100 + i;
   }

   int* outBuffer = (int*) PushBytes(arena,5 * sizeof(int));

   ClearCache(arena->mem);

   printf("A\n");

   UNIT_VRead_MemLoad(defaultData,4);
   UNIT_VRead_Simple(toLoad,4);

   printf("B\n");

   RunAccelerator(3);

   printf("C\n");

   UNIT_VRead_MemDump(outBuffer,4);

   printf("D\n");

   ClearCache(arena->mem);

   for(int i = 0; i < 4;i++){
      Assert_Eq(outBuffer[i],toLoad[i]);
   }
}
