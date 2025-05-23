#include "testbench.hpp"

void ConfigureVariety1(volatile EXAMPLE_Variety1Config* config,int* memory){
   VLinear_VRead(&config->read,memory,1);

   ConfigureSimpleMemory(&config->mem,1,0);
   VersatUnitWrite(TOP_simple_mem_addr,0,2);

   config->constant.constant = 1;
}

void SingleTest(Arena* arena){
   int memory = 3;

   accelConfig->input_0.constant = 5;
   accelConfig->input_1.constant = 4;

   ConfigureVariety1(&accelConfig->simple,&memory);

   RunAccelerator(2);

   Assert_Eq(accelState->output_0.currentValue,15);
}
