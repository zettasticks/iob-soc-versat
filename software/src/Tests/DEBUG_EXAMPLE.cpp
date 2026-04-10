#include "testbench.hpp"

void SingleTest(Arena* arena){
   RunAccelerator(3);

   VersatDebugState debug = VersatDebugGetState();

   Assert_Eq(debug.databusIsActive,0);
}
