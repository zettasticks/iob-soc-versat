#include "testbench.hpp"

void ConfigureVariety2(volatile EXAMPLE_Variety2Config* config,int* output,int* memory){
   VLinear_VRead(&config->read,memory,1);
   VLinear_Mem_Input_0(&config->mem,1);
   VLinear_VWrite(&config->write,output,1);

   VersatUnitWrite(TOP_simple_mem_addr,0,1);
}

void SingleTest(Arena* arena){
   int memory = 2;
   int* output = (int*) PushBytes(arena,sizeof(int));

   accelConfig->input_0.constant = 5;
   accelConfig->input_1.constant = 4;
   accelConfig->input_2.constant = 3;
   ConfigureVariety2(&accelConfig->simple,output,&memory);

   RunAccelerator(3);

   Assert_Eq(*output,15);
}