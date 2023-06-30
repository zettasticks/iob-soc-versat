#include "versat_accel.hpp"

#include "system.h"
#include "periphs.h"
#include "iob-uart.h"
#include "string.h"

#include "iob-timer.h"
#include "iob-ila.h"

int printf_(const char* format, ...);

#ifdef PC
#define uart_finish(...) ((void)0)
#define uart_init(...) ((void)0)
#else
#define printf printf_
#endif

int main(int argc,char* argv[]){
   uart_init(UART_BASE,FREQ/BAUD);
   timer_init(TIMER_BASE);
   ila_init(ILA_BASE);

   printf("Init base modules\n");

   versat_init(VERSAT_BASE);

   printf("%x\n",&ACCEL_TOP_input_0_constant);

   ACCEL_TOP_input_0_constant = 100;
   ACCEL_TOP_input_1_constant = 20;
   ACCEL_TOP_input_2_constant = 3;

   RunAccelerator(1);

   printf("%d\n",ACCEL_TOP_output_0_currentValue);

   uart_finish();

   return 0;
}









