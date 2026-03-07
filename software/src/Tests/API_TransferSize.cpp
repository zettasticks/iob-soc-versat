#include "testbench.hpp"

void SingleTest(Arena* arena){
   int data[8] = {0,1,2,3,4,5,6,7};
   int buffer[8] = {};

   VersatVarSpec width = {};
   width.min = 1;
   width.max = 8;
   width.order = 1;

   int size = API_TransferSize_Simple_Size(data,buffer,1<<15,&width);

   printf("%d %d\n",size,width.value);
}