#include "testbench.hpp"

#define SIZE 16

void SingleTest(Arena* arena){
   int *buffer = (int*) PushBytes(arena,sizeof(int) * SIZE);
   int *result = (int*) PushBytes(arena,sizeof(int) * SIZE);

   for(int i = 0; i < SIZE; i++){
      buffer[i] = i + 1;
   }
   API_DivWork_XVal_Struct val = API_DivWork_XVal();

   for(int i = 1; i <= SIZE; i *= 2){
      API_DivWork_InitMask(0x00000003);
      API_DivWork_Divide(SIZE,buffer,result);

      RunAccelerator(3);

      ClearCache(buffer);
      printf("Size: %d\n",i);
      for(int ii = 0; ii < i; ii++){
         printf("%d\n", result[ii]);
      }
      printf("\n\n\n");
   }
}
