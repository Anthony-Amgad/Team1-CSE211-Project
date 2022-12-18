#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "Lcd.h"
#include "Keypad.h"
#include "Timers.h"
#include "SysTimer.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

uint16_t state = 1;

uint16_t waits = 0;


//Calculator Variables
int64_t oper1 = 0;
uint16_t opermode = 0;
uint8_t calcin[14] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
uint16_t calciter = 0;
uint16_t oper1len = 0;


//Stopwatch Variables
uint64_t seconds = 0;


//Timer Variables
uint16_t timmode = 0;
uint8_t minin[4] = "0000";
uint8_t secin[2] = "00";
uint16_t timiter = 0;
uint64_t timremsec = 0;




void Timer1IntHandler(void){
    seconds++;
    if(state == 3 && waits){
    uint16_t stod = seconds%60;
    uint16_t mtod = seconds/60;
    int32_t length = snprintf( NULL, 0, "%hu", stod);
    char* str1 = malloc( length + 1 );
    snprintf(str1, length + 1, "%hu", stod );
    length = snprintf( NULL, 0, "%hu", mtod);
    char* str2 = malloc( length + 1 );
    snprintf(str2, length + 1, "%hu", mtod );
    str2 = strcat(str2,":");
    str1 = strcat(str1,"  ");
    LCD_PrintLn(0,strcat(str2,str1));
    free(str1);
    free(str2);
    }
    TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
 
}

void Timer2IntHandler(void){    
    if(timremsec > 0){
    timremsec--;
    
    if(state == 2 && waits){
    uint16_t stod = timremsec%60;
    uint16_t mtod = timremsec/60;
    
    int32_t length = snprintf( NULL, 0, "%hu", stod);
    char* str1 = malloc( length + 1 );
    snprintf(str1, length + 1, "%hu", stod );
    length = snprintf( NULL, 0, "%hu", mtod);
    char* str2 = malloc( length + 1 );
    snprintf(str2, length + 1, "%hu", mtod );
    str2 = strcat(str2,":");
    str1 = strcat(str1,"  ");
    LCD_PrintLn(0,strcat(str2,str1));
    free(str1);
    free(str2);
    }
    }else{
      LCD_Print("#   TIME OUT   #","#      !!      #");
      timmode = 2;
      state = 2;
      GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_3));
    }
    TimerIntClear(TIMER2_BASE,TIMER_TIMA_TIMEOUT);
}

void btnHandlers(void){
  switch(GPIOIntStatus(GPIO_PORTA_BASE,GPIO_INT_PIN_2)){ 
    
  case(GPIO_INT_PIN_2): //switch
    switch(state){
    case 1:
      opermode = 0;
      calciter = 0;
      oper1len = 0;
      oper1 = 0;
      memset(calcin, 0, sizeof(calcin));
      state = 2;
      break;
    case 2:
      //TimerDisable(TIMER2_BASE,TIMER_A);
      //GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0x0);
      //timmode = 0;
      //minin[0] = '0';
      //minin[1] = '0';
      //secin[0] = '0';
      //secin[1] = '0';
      //timiter = 0;
      //timremsec = 0;
      state = 3;
      break;
    case 3:
      //TimerDisable(TIMER1_BASE,TIMER_A);
      //seconds = 0;
      state = 1;      
      break;
    }
    while((GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_2) == GPIO_PIN_2));
    SysTimerDelay(50*16000);
    break;
  
  case(GPIO_INT_PIN_3): //start stopwatch
    if(state == 3){
          TimerEnable(TIMER1_BASE,TIMER_A);
      }
    break;
  
  case(GPIO_INT_PIN_4): //pause stopwatch
    if(state == 3){
          TimerDisable(TIMER1_BASE,TIMER_A);
      }
    break;
  
  case(GPIO_INT_PIN_5): //reset stopwatch
    if(state == 3){
          TimerDisable(TIMER1_BASE,TIMER_A);
          LCD_PrintLn(0,"0:0  ");
          seconds = 0;
      }
    break;
  
  }
  
  GPIOIntClear(GPIO_PORTA_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);  
  return;
}


void btns_init(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
  GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPD);
  GPIOIntDisable(GPIO_PORTA_BASE,GPIO_INT_PIN_2|GPIO_INT_PIN_3|GPIO_INT_PIN_4|GPIO_INT_PIN_5);
  IntMasterEnable();
  IntEnable(INT_GPIOA);
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
  
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));

  IntEnable(INT_TIMER1A);
  TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
  TimerIntRegister(TIMER1_BASE,TIMER_A,Timer1IntHandler);
  TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER1_BASE,TIMER_A,16000000);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2));

  IntEnable(INT_TIMER2A);
  TimerIntEnable(TIMER2_BASE,TIMER_TIMA_TIMEOUT);
  TimerIntRegister(TIMER2_BASE,TIMER_A,Timer2IntHandler);
  TimerConfigure(TIMER2_BASE,TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER2_BASE,TIMER_A,16000000);

  
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
  
  IntPrioritySet(INT_TIMER1A,0xE0);
  IntPrioritySet(INT_TIMER2A,0xE0);
  IntPrioritySet(INT_GPIOA,0xE0);


  
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3);
  
  
  
  uint8_t ch = '\0';
  
  uint8_t sw = 0;
  
  GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2));
  LCD_Print("     Team 1     "," CSE211 Project ");
  TimerDelay(3000);
  GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,~GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2));

  while (1) {
    
    switch(state){
    case 1:
      if(prevstate != 1){
        waits = 0;
        LCD_Print("#  Calculator  #","#     Mode     #");
        TimerDelay(1000);
        LCD_Clear();
        waits = 1;
        prevstate = 1;
      }
      
      
      while(state == 1){
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
            }else if(calciter == 0){
              if(calciter < 14 - oper1len){
              calcin[calciter] = '-';
              calciter++;
              LCD_Show('-');
              }else{
              LCD_Print("#   OVERFLOW   #","#      !!      #");
              TimerDelay(2000);
              LCD_Clear();
              opermode = 0;
              calciter = 0;
              oper1len = 0;
              oper1 = 0;
              memset(calcin, 0, sizeof(calcin));
              }
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
              }else if(opermode == 1){  // +
                int64_t result = oper1 + atoll(calcin);
                int32_t length = snprintf( NULL, 0, "%lld", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lld", result );
                LCD_PrintLn(1,str);
                free(str);
              }
              else if(opermode == 2){ // -
                int64_t result = oper1 - atoll(calcin);           
                int32_t length = snprintf( NULL, 0, "%lld", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lld", result );
                LCD_PrintLn(1,str);
                free(str);
              }
              else if(opermode == 3){ // /
                double result = (double)oper1 / (double)atoll(calcin);   
                int32_t length = snprintf( NULL, 0, "%lf", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lf", result );
                LCD_PrintLn(1,str);
                free(str);
              }
              else if(opermode == 4){ // *
                int64_t result = oper1 * atoll(calcin);
                int32_t length = snprintf( NULL, 0, "%lld", result);
                char* str = malloc( length + 1 );
                snprintf(str, length + 1, "%lld", result );
                LCD_PrintLn(1,str);
                free(str);
              }else if(opermode == 5){
                LCD_Clear();
                opermode = 0;
                calciter = 0;
                oper1len = 0;
                oper1 = 0;
                memset(calcin, 0, sizeof(calcin));
                break;
              }
              oper1len = 0;
              calciter = 1;
              oper1 = 0;
              memset(calcin, 0, sizeof(calcin));  
              opermode = 5;
            }
          }else{
            if((calciter < 14 - oper1len)  && (ch != '#')){
              calcin[calciter] = ch;
              calciter++;
              LCD_Show(ch);
              //printf("%s\n",calcin);
            }else if(ch != '#'){
              LCD_Print("#   OVERFLOW   #","#      !!      #");
              TimerDelay(2000);
              LCD_Clear();
              opermode = 0;
              calciter = 0;
              oper1len = 0;
              oper1 = 0;
              memset(calcin, 0, sizeof(calcin));
            }
          }
          sw = 1;
          break;
        }
      }

      while(sw && KeyPad_Read() != '\0');   
      sw = 0;      
      break;
      
    case 2:
      if(prevstate != 2){
        waits = 0;
        LCD_Print("#     Timer    #","#     Mode     #");
        TimerDelay(1000);
        if(timremsec == 0){
          LCD_Print("00:00           ","                ");
          LCD_Cursor(0,0);
        }else{
          LCD_Clear();
        }
        prevstate = 2;
        waits = 1;
      }
      while(state == 2){
        ch = KeyPad_Read(); 
        
        if ((ch != '\0') && sw == 0){
          if(timmode == 0){
            if((timiter < 2)  && (ch != '#') && (ch != '*') && (ch != 'A') && (ch != 'B') && (ch != 'C') && (ch != 'D')){
              minin[timiter] = ch;
              //printf("%s:%s",minin,secin);
              timiter++;
              LCD_Show(ch);
              if(timiter == 2){
                LCD_Show(':');
              }
              //printf("%s\n",calcin);
            }else if((timiter < 4) && (ch != '#') && (ch != '*') && (ch != 'A') && (ch != 'B') && (ch != 'C') && (ch != 'D')){
              secin[timiter-2] = ch;
              //printf("%s:%s",minin,secin);
              timiter++;
              LCD_Show(ch);
            }
            else if((timiter >= 4)  && (ch != '#') && (ch != '*') && (ch != 'A') && (ch != 'B') && (ch != 'C') && (ch != 'D')){
              timiter = 0;
              minin[0] = '0';
              minin[1] = '0';
              secin[0] = '0';
              secin[1] = '0';
              minin[timiter] = ch;
              //printf("%s:%s",minin,secin);
              LCD_PrintLn(0,"00:00");
              LCD_Cursor(0,0);
              LCD_Show(ch);
              timiter++;
            }
            else if(ch == 'D'){
              //printf("%s:%s",minin,secin);
              timremsec = atoll(secin);
              timremsec += (60*(atoll(minin)/100));
              timmode = 1;
              TimerEnable(TIMER2_BASE,TIMER_A);
            }
          }else if(timmode == 1 && ch == 'D'){
            TimerDisable(TIMER2_BASE,TIMER_A);
            timmode = 0;
            minin[0] = '0';
            minin[1] = '0';
            secin[0] = '0';
            secin[1] = '0';
            timiter = 0;
            timremsec = 0;
            LCD_Print("00:00           ","                ");
            LCD_Cursor(0,0);
          }else if(timmode == 2 && ch =='D'){
            TimerDisable(TIMER2_BASE,TIMER_A);
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0x0);
            timmode = 0;
            minin[0] = '0';
            minin[1] = '0';
            secin[0] = '0';
            secin[1] = '0';
            timiter = 0;
            timremsec = 0;
            LCD_Print("00:00           ","                ");
            LCD_Cursor(0,0);
          }
        }
          sw = 1;
          break;
        }
      

      while(sw && KeyPad_Read() != '\0');   
      sw = 0;      
      break;
      
    case 3:
      if(prevstate != 3){
        waits = 0;
        LCD_Print("#  Stopwatch   #","#     Mode     #");
        TimerDelay(1000);
        if(seconds == 0){
          LCD_Print("0:0             ","                ");
        }else{
          LCD_Clear();
        }
        prevstate = 3;
        waits = 1;
      }
      break;            
    }   
    
  }
  
  return 0;
}
