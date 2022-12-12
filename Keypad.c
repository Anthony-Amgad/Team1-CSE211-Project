#include <stdbool.h>
#include <stdint.h>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

void KeyPad_Init(){
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
  
  HWREG(GPIO_PORTE_BASE+GPIO_O_CR) |= 0x0F;
  HWREG(GPIO_PORTE_BASE+GPIO_O_DIR) = 0x0F;
  HWREG(GPIO_PORTE_BASE+GPIO_O_DEN) |= 0x0F;
  
  HWREG(GPIO_PORTC_BASE+GPIO_O_CR) |= 0xF0;
  HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) = 0x0;
  HWREG(GPIO_PORTC_BASE+GPIO_O_PDR) = 0xF0;
  HWREG(GPIO_PORTC_BASE+GPIO_O_DEN) |= 0xF0;
  
  /*GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPD);
  GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);*/
  
  return ;
  
}

uint8_t KeyPad_Read(){
  
  uint8_t arr[4][4] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
  uint16_t gparr[8] = {GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7};
  for(int t=0; t<10000; t++){   
    for(int i = 0; i<4;i++){
      GPIOPinWrite(GPIO_PORTE_BASE,gparr[i],gparr[i]);
      for(int j = 4; j<8; j++){
        if(GPIOPinRead(GPIO_PORTC_BASE,gparr[j]) == gparr[j]){ 
          return arr[i][j-4];
        }
      }
      GPIOPinWrite(GPIO_PORTE_BASE,gparr[i],0x0);
    }
  }
  
  return '\0';
  
}