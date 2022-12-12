#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "Lcd.h"
#include "Keypad.h"
#include "Timers.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

uint16_t state = 1;
double result = 0;
int64_t oper1 = 0;
uint16_t opermode = 0;
uint8_t calcin[14] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
uint16_t calciter = 0;
uint16_t oper1len = 0;


void btnHandlers(void){
  if(GPIOIntStatus(GPIO_PORTA_BASE,GPIO_INT_PIN_2) == GPIO_INT_PIN_2){ //switch
    switch(state){
    case 1:
      state = 2;
      break;
    case 2:
      state = 3;
      break;
    case 3:
      state = 1;
      break;
    }
  }else if (GPIOIntStatus(GPIO_PORTA_BASE,GPIO_INT_PIN_3) == GPIO_INT_PIN_3){ //start stopwatch
    
  }else if (GPIOIntStatus(GPIO_PORTA_BASE,GPIO_INT_PIN_4) == GPIO_INT_PIN_4){ //pause stopwatch
    
  }else if (GPIOIntStatus(GPIO_PORTA_BASE,GPIO_INT_PIN_5) == GPIO_INT_PIN_5){ //reset stopwatch
    
  }
  while((GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_2) == GPIO_PIN_2)|(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_3) == GPIO_PIN_3)|(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_4) == GPIO_PIN_4)|(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_5) == GPIO_PIN_5));
  GPIOIntClear(GPIO_PORTA_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);
  SysCtlDelay(50*16000);
  return;
}


void btns_init(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
  GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPD);
  GPIOIntDisable(GPIO_PORTA_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);
  IntMasterEnable();
  IntEnable(INT_GPIOA_TM4C123);
  GPIOIntRegister(GPIO_PORTA_BASE,btnHandlers);
  GPIOIntTypeSet(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_RISING_EDGE );
  GPIOIntEnable(GPIO_PORTA_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);

}



int main()
{
  uint16_t prevstate = 0;
  LCD_init();
  KeyPad_Init();
  TimerDelayinit();
  btns_init();
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
  
  
  
  uint8_t ch = '\0';
  
  uint8_t sw = 0;
  
  GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2));
  LCD_Print("     Team 1     "," CSE211 Project ");
  TimerDelay(4000);
  GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2));

  while (1) {
    
    switch(state){
    case 1:
      if(prevstate != 1){
        LCD_Print("#  Calculator  #","#     Mode     #");
        TimerDelay(3000);
        LCD_Clear();
        prevstate = 1;
      }
      while(1){
        ch = KeyPad_Read();     
        if ((ch != '\0') && sw == 0){
          if(ch == 'A'){   // +
            if(calciter != 0 && opermode == 0){   
              oper1len = calciter - 1;
              calciter = 0;
              oper1 = atoll(calcin);
              memset(calcin, 0, sizeof(calcin));
              opermode = 1;
              LCD_Show('+');
            }
          }else if(ch == 'B'){   // -
            if(calciter != 0  && opermode == 0){   
              oper1len = calciter - 1;
              calciter = 0;
              oper1 = atoll(calcin);
              memset(calcin, 0, sizeof(calcin));
              opermode = 2;
              LCD_Show('-');
            }
          }else if(ch == 'C'){   // /
            if(calciter != 0 && opermode == 0){   
              oper1len = calciter - 1;
              calciter = 0;
              oper1 = atoll(calcin);
              memset(calcin, 0, sizeof(calcin));
              opermode = 3;
              LCD_Show('/');
            }
          }else if(ch == '*'){   // *
            if(calciter != 0 && opermode == 0){   
              oper1len = calciter - 1;
              calciter = 0;
              oper1 = atoll(calcin);
              memset(calcin, 0, sizeof(calcin));
              opermode = 4;
              LCD_Show('X');
            }
          }else if(ch == 'D'){   // =
            if(calciter != 0){
              if(opermode == 0){
                LCD_PrintLn(1, calcin);
              }else if(opermode == 1){
                result = oper1 + atoll(calcin);
                int32_t length = snprintf( NULL, 0, "%lf", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lf", result );
                LCD_PrintLn(1,str);
                free(str);
              }
              else if(opermode == 2){
                result = oper1 - atoll(calcin);           
                int32_t length = snprintf( NULL, 0, "%lf", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lf", result );
                LCD_PrintLn(1,str);
                free(str);
              }
              else if(opermode == 3){
                result = (double)oper1 / (double)atoll(calcin);   
                int32_t length = snprintf( NULL, 0, "%lf", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lf", result );
                LCD_PrintLn(1,str);
                free(str);
              }
              else if(opermode == 4){
                result = oper1 * atoll(calcin);
                int32_t length = snprintf( NULL, 0, "%lf", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lf", result );
                LCD_PrintLn(1,str);
                free(str);
              }
            }
          }else{
            if(calciter < 14 - oper1len){
              calcin[calciter] = ch;
              calciter++;
              LCD_Show(ch);
              printf("%s\n",calcin);
            }else{
              LCD_Print("#   OVERFLOW   #","#      !!      #");
              TimerDelay(2000);
              LCD_Clear();
              memset(calcin, 0, sizeof(calcin));
              calciter = 0;
              opermode = 0;
            }
          }
          sw = 1;
          break;
        }
      }
      
      break;
      
    case 2:
      if(prevstate != 2){
        LCD_Print("#     Timer    #","#     Mode     #");
        prevstate = 2;
      }
      break;
      
    case 3:
      if(prevstate != 3){
        LCD_Print("#  Stopwatch   #","#     Mode     #");
        prevstate = 3;
      }
      break;            
    }
    

   
    /*int length = snprintf( NULL, 0, "%d", state);
        char* str = malloc( length + 1 );
        snprintf(str, length + 1, "%d", state );
        LCD_Print(str,"");
        free(str);*/
    
    /*if(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_2) == GPIO_PIN_2){
      LCD_Print("    switch    "," CSE211 Project ");
    }
    if(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_3) == GPIO_PIN_3){
      LCD_Print("    start    "," CSE211 Project ");
    }
    if(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_4) == GPIO_PIN_4){
      LCD_Print("    pause    "," CSE211 Project ");
    }
    if(GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_5) == GPIO_PIN_5){
      LCD_Print("    reset    "," CSE211 Project ");
    }*/
    
  while(sw && KeyPad_Read() != '\0');   
  sw = 0;
    
  
/*
    LCD_Print("Hello World!", "...................."); //Print 2 lines
    
    SysCtlDelay(80000000/3); //Delay
    LCD_Clear();

    LCD_PrintLn(0, "Team:");
    SysCtlDelay(8000000/3); //Delay

    LCD_PrintLn(1, "Team1");

    SysCtlDelay(80000000/3); //Delay
    LCD_Clear();

    LCD_Print("Hello", "World!");

    SysCtlDelay(80000000/3); //Delay
    LCD_Clear();*/
    
    }
  return 0;
}
