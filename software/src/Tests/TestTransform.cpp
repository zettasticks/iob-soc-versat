#define OUTPUT_RESULT

#include "simpleTest.hpp"

void SingleTest(Arena* arena){
   int input[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
   int output[] = {0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15};
   SimpleTest(input,output);
}