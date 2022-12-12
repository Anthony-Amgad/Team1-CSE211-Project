#include <stdbool.h>
#include <stdint.h>
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

uint8_t Tim0delayflag = true;

void Tim0Handler(void){
  Tim0delayflag = false;
  TimerIntClear(WTIMER0_BASE,TIMER_TIMA_TIMEOUT);
  return;
}

void TimerDelayinit(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WTIMER0));
  IntMasterEnable();
  IntEnable(INT_WTIMER0A);
  TimerIntEnable(WTIMER0_BASE,TIMER_TIMA_TIMEOUT);
  TimerIntRegister(WTIMER0_BASE,TIMER_A,Tim0Handler);
  TimerConfigure(WTIMER0_BASE,TIMER_CFG_ONE_SHOT);
}

void TimerDelay(uint32_t delay){
  TimerDisable(WTIMER0_BASE,TIMER_A);
  TimerLoadSet(WTIMER0_BASE,TIMER_A,delay*16000);
  TimerIntClear(WTIMER0_BASE,TIMER_TIMA_TIMEOUT); 
  TimerEnable(WTIMER0_BASE,TIMER_A);
  while(Tim0delayflag);
  Tim0delayflag = true;
}
