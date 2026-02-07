#include "testbench.hpp"

#define MAX_SIZE 10
#define SIZE 2

void SingleTest(Arena* arena){
   int* buffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);   
   int* expected = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);
   //            876543210
   int tests = 0b111111111;

   if(tests & (1 << 0)){
      int size = 2;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen(size);
      ADDRESSGEN_Simple_Write(buffer,size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      expected[0] = 0;
      expected[1] = 1;
      Assert_Eq(buffer,expected,MAX_SIZE,"Linear-Linear Size = 2");
   }

   if(tests & (1 << 1)){
      int size = 1;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen(size);
      ADDRESSGEN_Simple_Write(buffer,size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      expected[0] = 0;
      Assert_Eq(buffer,expected,MAX_SIZE,"Linear-Linear Size = 1");
   }

   if(tests & (1 << 2)){
      int size = 0;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen(size);
      ADDRESSGEN_Simple_Write(buffer,size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      Assert_Eq(buffer,expected,MAX_SIZE,"Linear-Linear Size = 0");
   }

   if(tests & (1 << 3)){
      int size = 2;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen2(size,size,size);
      ADDRESSGEN_Simple_Write(buffer,size * size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      expected[0] = 0;
      expected[1] = 1;
      expected[2] = 2;
      expected[3] = 3;
      Assert_Eq(buffer,expected,MAX_SIZE,"Double-Linear Size = 2");
   }

   if(tests & (1 << 4)){
      int size = 1;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen2(size,size,size);
      ADDRESSGEN_Simple_Write(buffer,size * size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      expected[0] = 0;
      Assert_Eq(buffer,expected,MAX_SIZE,"Double-Linear Size = 1");
   }

   if(tests & (1 << 5)){
      int size = 0;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen2(size,size,size);
      ADDRESSGEN_Simple_Write(buffer,size * size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      Assert_Eq(buffer,expected,MAX_SIZE,"Double-Linear Size = 1");
   }

   if(tests & (1 << 6)){
      int size = 2;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen(size * size);
      ADDRESSGEN_Simple_Write2(buffer,size,size,size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      expected[0] = 0;
      expected[1] = 1;
      expected[2] = 2;
      expected[3] = 3;
      Assert_Eq(buffer,expected,MAX_SIZE,"Linear-Double Size = 2");
   }

   if(tests & (1 << 7)){
      int size = 1;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen(size * size);
      ADDRESSGEN_Simple_Write2(buffer,size,size,size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      expected[0] = 0;
      Assert_Eq(buffer,expected,MAX_SIZE,"Linear-Double Size = 1");
   }

   if(tests & (1 << 8)){
      int size = 0;

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);

      ADDRESSGEN_Simple_Gen(size * size);
      ADDRESSGEN_Simple_Write2(buffer,size,size,size);

      RunAccelerator(3);
      ClearCache(buffer);

      ClearBuffer(expected,MAX_SIZE);
      Assert_Eq(buffer,expected,MAX_SIZE,"Linear-Double Size = 0");
   }
}