#include "testbench.hpp"

#define MAX_SIZE 2000
#define VALUES 1

void Test(int* buffer,int offset,int expected,const char* marker){
   ResetAccelerator();

   VLinear_Mem_Input_0(&accelConfig->output,VALUES);
   AddressGenConstant_VRead(&accelConfig->read,buffer,offset);

   RunAccelerator(3);
   Assert_Eq(VersatUnitRead(TOP_output_addr,0),expected,marker);
}

void SingleTest(Arena* arena){
   int* buffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE);

   ClearBuffer(buffer,MAX_SIZE);

   buffer[5+1]    = 100;
   buffer[5+10]   = 200;
   buffer[5+100]  = 300;
   buffer[5+1000] = 400;

   ClearCache(buffer);
   Test(buffer,1   ,100,"1");
   Test(buffer,10  ,200,"2");
   Test(buffer,100 ,300,"3");
   Test(buffer,1000,400,"4");
}