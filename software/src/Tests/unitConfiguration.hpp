#ifndef INCLUDED_UNIT_CONFIGURATION
#define INCLUDED_UNIT_CONFIGURATION

// TODO: This should go away. Change AddressGen to take over this and generate the functions 

#if 0

extern "C" {
   #include "printf.h" 
}

#ifdef VERSAT_DEFINED_VRead
static void ConfigureSimpleVReadBare(volatile VReadConfig* inst){
   IntSet(inst,0,sizeof(VReadConfig));

   // Memory side
   inst->pingPong = 1;

   // B - versat side
   inst->iter = 1;
   inst->incr = 1;
   inst->duty = 1;
}

static void ConfigureSimpleVReadShallow(volatile VReadConfig* inst, int numberItems,int* memory){
   inst->enabled = 1;

   // Memory side
   inst->ext_addr = (iptr) memory;
   inst->length = numberItems * sizeof(int);

   // B - versat side
   inst->per = numberItems;
   inst->duty = numberItems;
}

static void ConfigureSimpleVRead(volatile VReadConfig* inst, int numberItems,int* memory){
   ConfigureSimpleVReadBare(inst);
   ConfigureSimpleVReadShallow(inst,numberItems,memory);
}
#endif

#ifdef VERSAT_DEFINED_VWrite
static void ConfigureSimpleVWriteBare(volatile VWriteConfig* inst){
   IntSet(inst,0,sizeof(VWriteConfig));

   inst->pingPong = 1;

   inst->iter = 1;
   inst->duty = 1;
   inst->incr = 1;
}

static void ConfigureSimpleVWriteShallow(volatile VWriteConfig* inst, int numberItems,int* memory){
   inst->enabled = 1;

   // Write side
   inst->ext_addr = (iptr) memory;

   // Memory side
   inst->per = numberItems;
   inst->duty = numberItems;
}

static void ConfigureSimpleVWrite(volatile VWriteConfig* inst, int numberItems,int* memory){
   ConfigureSimpleVWriteBare(inst);
   ConfigureSimpleVWriteShallow(inst,numberItems,memory);
}
#endif

#endif

// CHANGE HERE
# if 0


#ifdef VERSAT_DEFINED_Mem
static void ConfigureSimpleMemory(volatile MemConfig* inst, int amountOfData, int start){
   IntSet(inst,0,sizeof(MemConfig));

   inst->iterA = 1;
   inst->perA = amountOfData;
   inst->dutyA = amountOfData;
   inst->incrA = 1;
   inst->startA = start;
}

static void ConfigureSimpleMemory(volatile MemConfig* inst, int amountOfData, int start,MemAddr addr,int* data){
   ConfigureSimpleMemory(inst,amountOfData,start);
   VersatMemoryCopy(addr.addr,data,amountOfData * sizeof(int));
}

static void ConfigureSimpleMemory(volatile MemConfig* inst, int amountOfData){
   IntSet(inst,0,sizeof(MemConfig));

   inst->iterA = 1;
   inst->perA = amountOfData;
   inst->dutyA = amountOfData;
   inst->incrA = 1;
}

static void ConfigureMemoryReceive(volatile MemConfig* inst, int amountOfData){
   IntSet(inst,0,sizeof(MemConfig));

   inst->iterA = 1;
   inst->perA = amountOfData;
   inst->dutyA = amountOfData;
   inst->incrA = 1;
   inst->in0_wr = 1;
}
#endif

#endif

#endif // INCLUDED_UNIT_CONFIGURATION