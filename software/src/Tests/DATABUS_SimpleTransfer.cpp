#include "testbench.hpp"

#include "unitConfiguration.hpp"

#define MAX_SIZE 5000
#define SIZE 1

bool DoARun(int size,int offset,int* inputBuffer,int* outputBuffer,Arena* arena){
  int* input = &inputBuffer[offset];
  int* output = &outputBuffer[offset];

  for(int i = 0; i < size; i++){
    input[i] = i + 1;
  }

  VLinear_VRead(&accelConfig->read,input,size);
  VLinear_VWrite(&accelConfig->write,output,size);
  
  RunAccelerator(1);

  accelConfig->read.enabled = 0;
  accelConfig->write.enabled = 0;

  RunAccelerator(2);

  ClearCache(arena->mem);

  bool failed = false;     
  for(int i = 0; i < size; i++){
    if(input[i] != output[i]){
      failed = true;
      printf("[%d:%d]Different at %d: %d %d\n",offset,size,i,input[i],output[i]);
    }
  }

  if(!failed){
    printf("[%d:%d] OK\n",offset,size);
  }

  return failed;
}

void* Align4096(void* ptr){
  iptr asInt = (iptr) ptr;

  iptr aligned = (asInt + 4095) & ~4095;

  return (void*) aligned;
}

// TODO: These sizes are in amount of units to transfer (which is DATA_W)
//       We also would like to test sizes that are not multiple of DATA_W, altought is not required currently.
int allSizes[] = {1,2,3,255,256,257,1023,1024,1025};

void SingleTest(Arena* arena){
  int* inputBuffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE * 2);
  int* outputBuffer = (int*) PushBytes(arena,sizeof(int) * MAX_SIZE * 2);

  bool failed = false;

#if 0
  char* inputBoundary  = (char*) PushBytes(arena,4096 * 4);
  char* outputBoundary = (char*) PushBytes(arena,4096 * 4);

  printf("A Input : %p\n",inputBoundary);
  printf("A Output: %p\n",outputBoundary);

  inputBoundary = (char*) Align4096(inputBoundary + 4097);
  outputBoundary = (char*) Align4096(outputBoundary + 4097);

  printf("B Input : %p\n",inputBoundary);
  printf("B Output: %p\n",outputBoundary);

  inputBoundary  += 4096 - 4 * 2;
  outputBoundary += 4096 - 4 * 2;

  printf("C Input : %p\n",inputBoundary);
  printf("C Output: %p\n",outputBoundary);

  // TODO: This test should offset the address by 1 (test unaligned copies)
  //       Since the CPU might not be able to handle unaligned accesses, need to write an 
  //       interface to allow easy filling of the array with the values that we want.

  printf("Input : %p\n",inputBoundary);
  printf("Output: %p\n",outputBoundary);
  DoARun(4,0,(int*) inputBoundary,(int*) outputBoundary,arena);
#endif

#if 1

#if 0 // Enable to run individual test
  printf("Gonna do a single run, wait a few seconds before terminating sim\n");
  failed |= DoARun(1025,0,inputBuffer,outputBuffer,arena);
#else
  for(int offset = 0; offset < 4; offset++){
    for(int sizeIndex = 0; sizeIndex < ARRAY_SIZE(allSizes); sizeIndex += 1){
      int size = allSizes[sizeIndex];

      failed |= DoARun(size,offset,inputBuffer,outputBuffer,arena);
    }
  }
#endif

#endif

  if(failed){
    Assert_Eq(0,1);
  } else {
    Assert_Eq(1,1);
  }
}
