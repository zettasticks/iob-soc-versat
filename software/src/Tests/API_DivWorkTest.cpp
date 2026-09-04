#include "testbench.hpp"

#define SIZE 16

void SingleTest(Arena* arena){
   int *buffer1 = (int*) PushBytes(arena,sizeof(int) * SIZE);
   int *buffer2 = (int*) PushBytes(arena,sizeof(int) * SIZE);

   API_DivWorkTest_Init(SIZE);
   API_DivWorkTest_Out(buffer1,buffer2,SIZE);

   RunAccelerator(3);

   for(int i = 0; i < SIZE; i++){
      printf("%d\n",buffer1[i]);
   }

   printf("\n\n");

   for(int i = 0; i < SIZE; i++){
      printf("%d\n",buffer2[i]);
   }
}
