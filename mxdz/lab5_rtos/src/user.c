#include <xc.h>          
#include <stdint.h>          
#include <sys/attribs.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "user.h"          
#include "OLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include <string.h>

uint8_t msg_1[16] = "Task 1         ";
uint8_t msg_2[16] = "Task 2         ";
uint32_t delay_g = 100;

void shift_str(uint8_t *str, uint8_t direction) 
{
    if (direction) {
        uint8_t tmp = str[strlen(str)-1];
        int i;
        for (i = strlen(str) - 1; i > 0; --i) {
            str[i] = str[i-1];
        }
        str[0] = tmp;
    } else {
        uint8_t tmp = str[0];
        int i;
        for (i = 0; i < strlen(str) - 1; ++i) {
            str[i] = str[i+1];
        }
        str[strlen(str)-1] = tmp;
    }
}

void InitGPIO(void) {
    // LED output
    // Disable analog mode for G6
    ANSELGbits.ANSG6 = 0;
    // Set direction to output for G6
    TRISGbits.TRISG6 = 0;

    // Initialize outputs for other LEDs
    ANSELBbits.ANSB11 = 0;
    ANSELGbits.ANSG15 = 0;

    TRISBbits.TRISB11 = 0;
    TRISGbits.TRISG15 = 0;
    TRISDbits.TRISD4 = 0;

    // Turn off LEDs for initialization
    LD1_PORT_BIT = 0;
    LD2_PORT_BIT = 0;
    LD3_PORT_BIT = 0;
    LD4_PORT_BIT = 0;

    // Initilalize input for BTN1
    // Disable analog mode
    ANSELAbits.ANSA5 = 0;
    // Set direction to input
    TRISAbits.TRISA5 = 1;

    // Initialize input for BTN2
    TRISAbits.TRISA4 = 1;
}

void InitBIOSGPIO(void) {
    /* Setup functionality and port direction */
    // LED outputs
    // Disable analog mode
    // Set directions to output
    TRISE = 0;
   
    // Test LEDs
    LATE = 0xff;        
    
    // Turn off LEDs for initialization
    LATE = 0;

    // Button inputs
    ANSELGbits.ANSG7 = 0;
    
    
    
    ANSELEbits.ANSE8 = 0;
    ANSELEbits.ANSE8 = 0;
 
    ANSELCbits.ANSC1 = 0;
    
    // Set directions to input
    TRISGbits.TRISG7 = 1;
    TRISDbits.TRISD5 = 1;
    TRISFbits.TRISF1 = 1;
    TRISAbits.TRISA2 = 1; 
    
    TRISEbits.TRISE8 = 1;
    TRISEbits.TRISE9 = 1;
    TRISAbits.TRISA14 = 1;
    TRISCbits.TRISC1 = 1; 
}

void InitApp(void) {
    // Initialize peripherals
    InitGPIO();
    InitBIOSGPIO();
    
    OledHostInit();
    OledDspInit();
    OledDvrInit();
}

void Task1(void * pvParameters) {
    
    while (1) {
        if (!BIOS_SW3_PORT_BIT)
        {
        xSemaphoreTake(xMutexOLED, portMAX_DELAY);
        OledSetCursor(0,3);
        OledPutString("GNU/FreeRTOS");
        OledSetCursor(0,1);
        OledPutString(msg_1);
        DelayMs(delay_g);
        shift_str(msg_1, 0);
        xSemaphoreGive(xMutexOLED);
        vTaskDelay(1); 
        }
    }
}

void Task2(void * pvParameters) {
    
    while (1) {
        xSemaphoreTake(xMutexOLED, portMAX_DELAY);
      OledSetCursor(0,3);
      OledPutString("GNU/FreeRTOS");
      if (BIOS_SW3_PORT_BIT)
        {
            OledSetCursor(0,1);
            OledPutString(msg_2);
            DelayMs(delay_g);
            shift_str(msg_2, 1);
        }
        xSemaphoreGive(xMutexOLED);
        vTaskDelay(1);
    }
}

void DelayMs(int t) {
    volatile long int count = t*33356;
    while (count--)
        ;
}