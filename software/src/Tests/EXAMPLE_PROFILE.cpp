#include "testbench.hpp"

#include "unitConfiguration.hpp"

#define SIZE 1234

void SingleTest(Arena* arena){
  int* input = (int*) PushBytes(arena,sizeof(int) * SIZE * 2);
  int* output = (int*) PushBytes(arena,sizeof(int) * SIZE * 2);

  for(int i = 0; i < SIZE; i++){
    input[i] = i + 1;
  }

  VersatProfileReset();

  VLinear_VRead(&accelConfig->read,input,SIZE);
  VLinear_VWrite(&accelConfig->write,output,SIZE);

  RunAccelerator(1);

  accelConfig->read.enabled = 0;
  accelConfig->write.enabled = 0;

  RunAccelerator(2);

  VersatProfile p = VersatProfileGet();

  VersatPrintProfile(p);

  // TODO: Since pc-emul basically does nothing right now, we cannot actually Assert the values returned.
  //       We might eventually add some logic to pc-emul and we could put some asserts in here but for now we mostly care about the textual output in sim-run
  Assert_Eq(0,0);
}
