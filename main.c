#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#include "Keypad.h"

#include "driverlib/sysctl.h"


int main()
{
  
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
  LCD_init();
  KeyPad_Init();
  
  uint8_t ch[2] = "\0";
  uint8_t sw = 0;
  
  while (1) {
    
    ch[0] = '\0';
    for(int i = 0; i< 100000; i++){
      ch[0] = KeyPad_Read();
      printf("%s",ch);
      if ((ch[0] != '\0') && sw == 0){
        LCD_Clear();
        sw = 1;
        LCD_Print(ch,"");
        break;
      }
    }
    
    
  while(sw && KeyPad_Read() != 0);   
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
