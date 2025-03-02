#include "testbench.hpp"

void SingleTest(Arena* arena){
   {
      ActivateMergedAccelerator(MergeType_FullShare);

      accelConfig->FullShare.unitsX_0.constant = 5;
      accelConfig->FullShare.unitsX_1.constant = 10;
      accelConfig->FullShare.unitsX_2.constant = 15;

      RunAccelerator(3);

      Assert_Eq(accelState->FullShare.output_0.currentValue,15);
      Assert_Eq(accelState->FullShare.output_1.currentValue,15);
      Assert_Eq(accelState->FullShare.output_2.currentValue,15);
   }

   {
      ActivateMergedAccelerator(MergeType_SomeShare);

      accelConfig->SomeShare.unitsY_0.constant = 50;
      accelConfig->SomeShare.unitsY_1.constant = 100;
      accelConfig->SomeShare.nonShared.constant = 150;

      RunAccelerator(3);

      Assert_Eq(accelState->SomeShare.output_0.currentValue,100);
      Assert_Eq(accelState->SomeShare.output_1.currentValue,100);
      Assert_Eq(accelState->SomeShare.output_2.currentValue,150);
   }

   {
      ActivateMergedAccelerator(MergeType_NoShare);

      accelConfig->NoShare.unitsZ_0.constant = 1;
      accelConfig->NoShare.unitsZ_1.constant = 2;
      accelConfig->NoShare.unitsZ_2.constant = 3;

      RunAccelerator(3);

      Assert_Eq(accelState->NoShare.output_0.currentValue,1);
      Assert_Eq(accelState->NoShare.output_1.currentValue,2);
      Assert_Eq(accelState->NoShare.output_2.currentValue,3);
   }
}