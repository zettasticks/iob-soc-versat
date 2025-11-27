#include "testbench.hpp"

#define MAX_BUFFER (1024 * 1024)

static int buffer[MAX_BUFFER];

#define NORM(VAL) ((VAL == 0 ? 1 : VAL))

static bool errorThis = false;
void SimulateAndCompare(AddressGenArguments* args,int* buffer){
   VersatAddressSimState iterInst = StartAddressSimulation(args);
   VersatAddressSimState* iter = &iterInst;

   for(;IsValid(iter); Advance(iter)){
      int index = GetIndex(iter);
      int address = GetAddress(iter);

      if(address != buffer[index]){
         errorThis = true;
         printf("Different: %d %d %d\n",address,buffer[index],index);
      }
   }
}

void SingleTest(int xStride,int xMax,int yStride,int yTotalStride,int yMax,int zStride,int zTotalStride,int zMax){
   int size = NORM(xMax) * NORM(yMax) * NORM(zMax);

   AddressGenArguments args = CompileVUnit_FullAddressGen(xStride,xMax,yStride,yTotalStride,yMax,zStride,zTotalStride,zMax);
   LoadVUnit_Generator(&accelConfig->gen,args);

   VLinear_VWrite(&accelConfig->write,buffer,size);

   RunAccelerator(3);

   SimulateAndCompare(&args,buffer);
}

static int argumentsMax[] = {4,4,4,4,4,4,4,4};

bool IncrementArgument(Array<int> args,Array<int> max){
   bool finished = true;
   for(int i = 0; i < max.size; i++){
      if(args.data[i] < max.data[i]){
         finished = false;
         args.data[i] += 1;
         break;
      } else if(args.data[i] >= max.data[i]){
         args.data[i] = 0;
      }
   }

   return finished;
}

void SingleTest(Arena* arena){
   // Go check any of the specific tests to get a more useful template.
   // ADDRESSGEN if doing something involving heavy use of address gen and so on.

   //SingleTest(2,2,2,2,2,2,2,2);

#if 0
   SingleTest(2,2,2,2,2,2,2,2);
   //SingleTest(1,2,0,1,0,0,1,0);
#else
   int size = ARRAY_SIZE(argumentsMax);
   Array<int> args = PushArray<int>(arena,size);
   Array<int> max = {argumentsMax,size};

   bool finished = false;
   while(!finished && !errorThis){
      for(int i = 0; i < size; i++){
         printf("%d ",args[i]);
      }
      printf("\n");

      SingleTest(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]);
      finished |= IncrementArgument(args,max);
   }

   Assert_Eq(0,0);
#endif
}
