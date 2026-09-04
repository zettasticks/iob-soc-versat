#include "testbench.hpp"

void Test(int* inBuffer,int* buffer,int amount,int unitCount,int index){
   int zeros[100] = {};
   API_Test_Clear(zeros,amount);

   API_Test_Init(inBuffer,amount,unitCount,index);
   API_Test_InitMem(amount,unitCount,index);

   RunAccelerator(1);

   API_Test_DisableMem();
   VERSAT_DisableReadsAndWrites();

   RunAccelerator(2);

   API_Test_Read(buffer,amount);
   ClearCache(buffer);
   for(int i = 0; i < amount; i++){
      printf("%d\n",buffer[i]);
   }
   printf("\n\n");
}

void SingleTest(Arena* arena){
   int* inBuffer = (int*) PushBytes(arena,sizeof(int) * 100);
   int* buffer = (int*) PushBytes(arena,sizeof(int) * 100);

   for(int i = 0; i < 100; i++){
      inBuffer[i] = i + 1;
   }

   if(1){
      printf("A0\n");
      Test(inBuffer,buffer,3,2,0);
      Test(inBuffer,buffer,3,2,1);

      printf("A1\n");
      Test(inBuffer,buffer,4,2,0);
      Test(inBuffer,buffer,4,2,1);
   }

   // Only 1 work meaning that unit 0 should do it and the rest should produce zero.
   if(1){
      printf("A2\n");
      Test(inBuffer,buffer,1,4,0);
      Test(inBuffer,buffer,1,4,1);
      Test(inBuffer,buffer,1,4,2);
      Test(inBuffer,buffer,1,4,3);
   }

   if(1){
      // 1 work in a 4 partition means that only unit 0 does 1 work
      printf("A3.1\n");
      Test(inBuffer,buffer,1,4,0);
      Test(inBuffer,buffer,1,4,1);
      Test(inBuffer,buffer,1,4,2);
      Test(inBuffer,buffer,1,4,3);

      // 2 work in a 4 partition means that unit 0 and 1 does 1 work
      printf("A3.2\n");
      Test(inBuffer,buffer,2,4,0);
      Test(inBuffer,buffer,2,4,1);
      Test(inBuffer,buffer,2,4,2);
      Test(inBuffer,buffer,2,4,3);

      // Only unit 4 does no work
      printf("A3.3\n");
      Test(inBuffer,buffer,3,4,0);
      Test(inBuffer,buffer,3,4,1);
      Test(inBuffer,buffer,3,4,2);
      Test(inBuffer,buffer,3,4,3);

      // All units do 1 work
      printf("A3.4\n");
      Test(inBuffer,buffer,4,4,0);
      Test(inBuffer,buffer,4,4,1);
      Test(inBuffer,buffer,4,4,2);
      Test(inBuffer,buffer,4,4,3);

      // Unit0 does 2 work and rest only 1
      printf("A3.5\n");
      Test(inBuffer,buffer,5,4,0);
      Test(inBuffer,buffer,5,4,1);
      Test(inBuffer,buffer,5,4,2);
      Test(inBuffer,buffer,5,4,3);
   }
}
