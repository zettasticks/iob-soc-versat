#include "testbench.hpp"

void SingleTest(Arena* arena){
   API_Simple_Init(123);

   RunAccelerator(3);

   API_Simple_Result_Struct r = API_Simple_Result();

   printf("%d\n",r.res);

   Assert_Eq(r.res,123);
}