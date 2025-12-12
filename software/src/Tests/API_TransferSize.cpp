#include "testbench.hpp"

/*
   The most important part of the whole process is to be able to support different sized memories automatically.

   - Which means that we need to be able to return some info upwards that allow us to calculate some variables than change depending on the amount of memory available.

   - Some values are constant. No matter what happens they are always given fixed.
   - Other values are changeable. 
   -- For the convolution based on the window approach, the width and heigh of the window is variable. The kernel size and input channels and other stuff is fixed. If a window of width 1 and height 1 cannot accomodate the fixed parameters then the problem is that the memory is too small and there is nothing else that we can do.

   - But how do we handle more than 1 variable parameter?
   -- If we only have one then the problem is easy, for a given combination of fixed and memory size we can just return the maximum value of the variable parameter.

   -- But for 2 parameters then it is more difficult. We could fix each parameter to 1 at a time and then return as if we only have one variable parameter.
   -- We could try to instruct Versat to return the maximum of both parameters at the same time using some metric.
   -- We could have the function receive the min and max of a given attribute as well as some way of specifying priority and then the function tries to maximize based on the priority given (which would work great for the convolution, we want to maximize x before y).
   -- We could generate one function for each combination where all the variable attributes are fixed except one and then the runtime uses that function to find the best parameter for itself by iterating and trying different fixed values.

   We also want to be able to make it so it can scale with a different amount of units.
   Meaning that the functions must support either 1 or N values depending on the amount of units that we want to instantiate.
*/

void SingleTest(Arena* arena){
   int data[8] = {0,1,2,3,4,5,6,7};
   int buffer[8] = {};

   VersatVarSpec width = {};
   width.min = 1;
   width.max = 8;
   width.order = 1;

   int size = API_TransferSize_Simple2_Size(data,buffer,1<<15,&width);

   printf("%d %d\n",size,width.value);
}