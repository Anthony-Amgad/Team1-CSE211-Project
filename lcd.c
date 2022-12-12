#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "string.h"

#include "SysTimer.h"


#define RS              GPIO_PIN_0
#define E               GPIO_PIN_1
#define D4              GPIO_PIN_4
#define D5              GPIO_PIN_5
#define D6              GPIO_PIN_6
#define D7              GPIO_PIN_7

void LCD_Command(unsigned char c) {

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (c & 0xf0) );
        GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x00);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);

        SysTimerDelay(25000);

        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);

        SysTimerDelay(25000);

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (c & 0x0f) << 4 );
        GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x00);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);

        SysTimerDelay(10);

        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);

        SysTimerDelay(25000);

}

void LCD_Clear(void){

        LCD_Command(0x01);
        SysTimerDelay(10);

}

void LCD_init() {
        SysTimerDelayinit();
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
        GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);

        SysTimerDelay(25000);

        GPIOPinWrite(GPIO_PORTB_BASE, RS,  0x00 );

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x30 );
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
        SysTimerDelay(10);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);

        SysTimerDelay(25000);

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x30 );
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
        SysTimerDelay(10);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);

        SysTimerDelay(25000);

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x30 );
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
        SysTimerDelay(10);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);

        SysTimerDelay(25000);

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x20 );
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
        SysTimerDelay(10);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);

        SysTimerDelay(25000);


        LCD_Command(0x0F); //Turn on Lcd
        LCD_Clear(); //Clear screen

}

void LCD_Show(unsigned char d) {

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (d & 0xf0) );
        GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x01);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);

        SysTimerDelay(10);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
        SysTimerDelay(25000);

        GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (d & 0x0f) << 4 );
        GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x01);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);

        SysTimerDelay(10);
        GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
        SysTimerDelay(25000);

}

void LCD_Cursor(char x, char y){

    if (x==0) {
        LCD_Command(0x80 + (y % 16));
        return;
    }
    LCD_Command(0xC0 + (y % 16));

}

void LCD_Print(char *s, char *d) {
    int j;
    for (j=0; j<16; j++) {
        if (j<strlen(s)) {
            LCD_Cursor(0,j);
            LCD_Show(s[j]);
        }
        if (j<strlen(d)) {
            LCD_Cursor(1,j);
            LCD_Show(d[j]);
        }
    }
    LCD_Command(0xC0 + 16); //Hide cursor
}

void LCD_PrintLn(char i, char *s) {
    LCD_Cursor(i, 0);
    for (i=0; i<strlen(s); i++) {
        LCD_Show(s[i]);
    }
    LCD_Command(0xC0 + 16); //Hide cursor
}