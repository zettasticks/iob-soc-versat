#ifndef INCLUDED_UNIT_CONFIGURATION
#define INCLUDED_UNIT_CONFIGURATION

// TODO: This should go away. Change AddressGen to take over this and generate the functions 
// CHANGE HERE
#if 0
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