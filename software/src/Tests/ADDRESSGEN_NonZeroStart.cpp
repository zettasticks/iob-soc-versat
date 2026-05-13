#include "testbench.hpp"

void SingleTest(Arena* arena){
   ADDRESSGEN_NonZeroStart_Init();
   RunAccelerator(3);

   Assert_Eq(ADDRESSGEN_NonZeroStart_Result().res,5);
}
