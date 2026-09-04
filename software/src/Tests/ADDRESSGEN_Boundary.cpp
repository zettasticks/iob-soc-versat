#include "testbench.hpp"

#define MAX_SIZE 10
#define VALUES 4

void SingleTest(Arena* arena){
   int* buffer = (int*) PushBytes(arena,sizeof(int));
   int* read = (int*) PushBytes(arena,sizeof(int));
   buffer[0] = 123;

   ADDRESSGEN_Boundary_Write(buffer,1);
   ADDRESSGEN_Boundary_Init(0);

   RunAccelerator(3);

   ADDRESSGEN_Boundary_Read(read,1);

   Assert_Eq(read[0],123);
}