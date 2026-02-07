#include "testbench.hpp"

#define MAX_SIZE 100
#define VALUES 8
#define VALUES_2 20

void SingleTest(Arena* arena){
   int* buffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);

   if(1){
      int expectedValues[] = {10,10,20,20,30,30,40,40};
   
      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[2]  = 20;
      buffer[5]  = 30;
      buffer[7]  = 40;
      
      ADDRESSGEN_EmptyLoops_Mem(VALUES);
      ADDRESSGEN_EmptyLoops_EmptyLoops(buffer);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[MAX_SIZE];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"1");
   }

   if(1){
      int expectedValues[] = {10,20,10,20,30,40,30,40};

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[2]  = 20;
      buffer[5]  = 30;
      buffer[7]  = 40;
      
      ADDRESSGEN_EmptyLoops_Mem(VALUES);
      ADDRESSGEN_EmptyLoops_EmptyLoops2(buffer);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[MAX_SIZE];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"2");
   }

   if(1){
      int expectedValues[] = {10,20,30,40,10,20,30,40};

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[2]  = 20;
      buffer[5]  = 30;
      buffer[7]  = 40;
      
      ADDRESSGEN_EmptyLoops_Mem(VALUES);
      ADDRESSGEN_EmptyLoops_EmptyLoops3(buffer);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[MAX_SIZE];
      for(int i = 0; i < VALUES; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES,"3");
   }   
   
   if(1){
      int expectedValues[] = {10,10,10,10,10,20,20,20,20,20,30,30,30,30,30,40,40,40,40,40};

      ResetAccelerator();
      ClearBuffer(buffer,MAX_SIZE);
      buffer[0]  = 10;
      buffer[1]  = 20;
      buffer[2]  = 30;
      buffer[3]  = 40;

      ADDRESSGEN_EmptyLoops_Mem(VALUES_2);
      ADDRESSGEN_EmptyLoops_EmptyLoops4(buffer);

      ClearCache(buffer);
      RunAccelerator(3);

      int result[MAX_SIZE];
      for(int i = 0; i < VALUES_2; i++){
         result[i] = VersatUnitRead(TOP_output_addr,i);
      }

      Assert_Eq(result,expectedValues,VALUES_2,"4");
   }
}