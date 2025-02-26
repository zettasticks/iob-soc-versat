#include "testbench.hpp"

void SingleTest(Arena* arena){
   int input[] = {1,2,3,4,0,0,0,0,5,6,7,8,0,0,0,0,9,10,11,12,0,0,0,0,13,14,15,16};
   int expected[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

   int* pointers[100];

   // Read array phased and use linear address gen
   if(1){
      Configure_VReadMultipleLoad_VReadMultiple(&accelConfig->read,(iptr) input,4,4,8);
      Configure_VReadLinear_VReadMultiple(&accelConfig->read,16);
      
      int totalSamples = Simulate_VReadMultipleLoad_VReadMultiple((iptr*) pointers,100,&accelConfig->read);
      Assert_Eq(totalSamples,ARRAY_SIZE(expected));

      for(int i = 0; i < totalSamples; i++){
         int* val = (int*) pointers[i];
      }

      for(int i = 0; i < ARRAY_SIZE(expected); i++){
         Assert_Eq(*pointers[i],expected[i]);
      }
   }

   // Read array linearly and use phased address gen
   if(1){  
      // Read the entire array
      Configure_VReadMultipleLoad_VReadMultiple(&accelConfig->read,(iptr) input,32,1,0);
      // But use address gen to only get the values that we expect.
      Configure_VReadMultipleOutputPhased_VReadMultiple(&accelConfig->read,4,4,8);

      int totalSamples = Simulate_VReadMultipleLoad_VReadMultiple((iptr*) pointers,100,&accelConfig->read);
      Assert_Eq(totalSamples,ARRAY_SIZE(expected));

      for(int i = 0; i < totalSamples; i++){
         int* val = (int*) pointers[i];
      }

      for(int i = 0; i < ARRAY_SIZE(expected); i++){
         Assert_Eq(*pointers[i],expected[i]);
      }
   // TODO: We probably want to simulate more types of Address Gens. We only test one here and it's only for a VReadMultiple.
   }

   if(1){
      Configure_VReadMultipleLoad_VReadMultiple(&accelConfig->read,(iptr) input,0,0,0);
      Configure_VReadLinear_VReadMultiple(&accelConfig->read,0);
      int totalSamples = Simulate_VReadMultipleLoad_VReadMultiple((iptr*) pointers,100,&accelConfig->read);

      Assert_Eq(totalSamples,0);
   }

   if(1){
      Configure_VReadMultipleLoad_VReadMultiple(&accelConfig->read,(iptr) input,1,1,1);
      Configure_VReadLinear_VReadMultiple(&accelConfig->read,1);
      int totalSamples = Simulate_VReadMultipleLoad_VReadMultiple((iptr*) pointers,100,&accelConfig->read);

      Assert_Eq(totalSamples,1);
   }

}
