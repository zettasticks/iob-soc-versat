#include "testbench.hpp"

void SingleTest(Arena* arena){
   int* loadBuffer = (int*) PushBytes(arena,sizeof(int) * 4);
   int* dumpBuffer = (int*) PushBytes(arena,sizeof(int) * 4);

   for(int i = 0; i < 4; i++){
      loadBuffer[i] = i + 1;
   }

   API_Mem_Linear(4);
   API_Mem_Load(loadBuffer,4);

   RunAccelerator(3);

   API_Mem_Dump(dumpBuffer,4);

   ClearCache(arena->mem);

   for(int i = 0; i < 4; i++){
      Assert_Eq(i + 1,dumpBuffer[i]);
   }
}