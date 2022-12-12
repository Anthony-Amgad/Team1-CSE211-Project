#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "Lcd.h"
#include "Keypad.h"
#include "Timers.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

int main()
{
  
  LCD_init();
  KeyPad_Init();
  TimerDelayinit();
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
  
  
  
  uint8_t ch[2] = "\0";
  uint8_t sw = 0;
  
  GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2));
  LCD_Print("    Team  17    "," CSE211 Project ");
  TimerDelay(4000);
  GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2));
  LCD_Print("To switch to dif"," modes press  # ");
  TimerDelay(4000);
  
  while (1) {
    
    while(1){
      ch[0] = KeyPad_Read();     
      if ((ch[0] != '\0') && sw == 0){
        LCD_Clear();
        sw = 1;
        LCD_Print(ch,"");
        break;
      }
    }
    
    
  while(sw && KeyPad_Read() != '\0');   
  sw = 0;
    
  
/*
    LCD_Print("Hello World!", "...................."); //Print 2 lines
    
    SysCtlDelay(80000000/3); //Delay
    LCD_Clear();

    LCD_PrintLn(0, "GITHUB:");
    SysCtlDelay(8000000/3); //Delay

    LCD_PrintLn(1, "ahmeterenodaci");

    SysCtlDelay(80000000/3); //Delay
    LCD_Clear();

    LCD_Print("Hello", "World!");

    SysCtlDelay(80000000/3); //Delay
    LCD_Clear();*/
    
    }
  return 0;
}
