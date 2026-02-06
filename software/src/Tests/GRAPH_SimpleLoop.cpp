#include "testbench.hpp"

void SingleTest(Arena* arena){
   GRAPH_SimpleLoop_Simple(5);

   //accelConfig->input_0.constant = 5;
   RunAccelerator(3);

   Assert_Eq(GRAPH_SimpleLoop_Result().res,5);
 
   RunAccelerator(1);
   Assert_Eq(GRAPH_SimpleLoop_Result().res,10);
 
   RunAccelerator(1);
   Assert_Eq(GRAPH_SimpleLoop_Result().res,15);
}