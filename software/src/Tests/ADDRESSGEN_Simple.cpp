#include "testbench.hpp"

void SingleTest(Arena* arena){
   int xMax = 2;
   int yMax = 2;
   int zMax = 2;
   int wMax = 2;
   int aMax = 2;

   int total = xMax * yMax * zMax * wMax * aMax;

   int expected[] = {0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3};

   TestAddressGen_Generator(&accelConfig->gen,xMax,yMax,zMax,wMax,aMax);
   VLinear_Mem_Input_0(&accelConfig->output,total);

   RunAccelerator(3);

   for(int i = 0; i < total; i++){
      Assert_Eq(expected[i],VersatUnitRead(TOP_output_addr,i));
   }
}