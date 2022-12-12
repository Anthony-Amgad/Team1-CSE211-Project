#include <stdbool.h>
#include <stdint.h>
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"

uint8_t SysTdelayflag = true;

void SysTimHandler(void){
  SysTickDisable();
  SysTdelayflag = false;
  return;
}

void SysTimerDelayinit(){
  SysTickIntRegister(SysTimHandler);
}

void SysTimerDelay(uint32_t delay){
  SysTickDisable();
  SysTickPeriodSet(delay);
  SysTickEnable();
  while(SysTdelayflag);
  SysTdelayflag = true;
}

