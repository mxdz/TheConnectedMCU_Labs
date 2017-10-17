#include "morse_code.h"      

void morse_code_encoding(char msg[], char *res) 
{
    char *morse_table[] = {
        "13","3111","3131","311","1","1131","331","1111","11","1333","313","1311","33","31","333","1331","3313","131","111","3","113","1113","133","3113","3133","3311"
    };
    for (uint32_t i = 0; msg[i]!='\0'; i++) {
        if (msg[i] == ' ') {
            strcat(res, " ");
        } else {
            int pos = msg[i] - 'a';
            strcat(res, morse_table[pos]);
        }
    }
}

void init_app(void) 
{
    /* Setup analog functionality and port direction */
    // LED outputs
    // Disable analog mode if present
    ANSELGbits.ANSG6 = 0;
    ANSELBbits.ANSB11 = 0;
    ANSELGbits.ANSG15 = 0;
    // Set direction to output 
    TRISGbits.TRISG6 = 0;
    TRISBbits.TRISB11 = 0;
    TRISGbits.TRISG15 = 0;
    TRISDbits.TRISD4 = 0;
    // Turn off LEDs for initialization
    LED_1 = LED_2 = LED_3 = LED_4 = 0;
    // BTN1 and BTN2 inputs
    // Disable analog mode
    ANSELAbits.ANSA5 = 0;
    // Set directions to input
    TRISAbits.TRISA5 = 1;
    TRISAbits.TRISA4 = 1;
    // 3. Configure peripheral to generate interrupts
    // Enable change notification interrupt in CN
    CNENAbits.CNIEA5 = 1;
    CNENAbits.CNIEA4 = 1;
    // 4. Configure Interrupt Controller
    // Enable change notification  interrupts
    IEC3bits.CNAIE = 1;
    // Set priority
    IPC29bits.CNAIP = 2;
    // Clear interrupt Flag
    IFS3bits.CNAIF = 0;
    // 5. Set Interrupt Controller for multi-vector mode
    INTCONSET = _INTCON_MVEC_MASK;
    // 6. Globally enable interrupts
    __builtin_enable_interrupts();
    // 7. Enable peripheral
    CNCONAbits.ON = 1;
    
    morse_code_encoding(MSG, encoded_msg_g);
    cur_state_g = RESET;
    cur_delay_g = BLINK_DELAY;
}

void delay(volatile uint32_t val) 
{
    val *= 10000;
    while (val --> 0);
}

void blink_leds(uint32_t mode) 
{
    if (mode == 1) {
        LED_1 = 1;
        delay(cur_delay_g);
        while(cur_state_g == PAUSE);
        LED_1 = 0;
        delay(cur_delay_g);
    } else if(mode == 3) {
        LED_2 = LED_3 = LED_4 = 1;
        delay(cur_delay_g);
        while(cur_state_g == PAUSE);
        LED_2 = LED_3 = LED_4 = 0;
        delay(cur_delay_g);
    }
}

void display_msg(void) 
{
    cur_delay_g = BLINK_DELAY;
    if (cur_state_g == START) { 
        for(uint32_t i = 0; encoded_msg_g[i]!='\0'; i++) {
            if (cur_state_g == RESET) {
                break;
            }
            if (encoded_msg_g[i] == '1') {
                blink_leds(1);
            } else if (encoded_msg_g[i] == '3') {
                blink_leds(3);
            } else {
                delay(cur_delay_g);
            }
        }
        delay(BLINK_DELAY);
        cur_state_g = RESET;
    }
}

void __ISR(_CHANGE_NOTICE_A_VECTOR, IPL2SOFT) ISR_PortA_Change(void) 
{
    if (CNSTATAbits.CNSTATA4 == 1) { // Bit 4 (BTN2) changed
        if (BTN_2)  { 
            delay(DEBOUNCE_DELAY);
            cur_state_g = PAUSE;
        }
    }
    if (CNSTATAbits.CNSTATA5 == 1) { // Bit 5 (BTN1) changed
        if (BTN_1)  { 
            delay(DEBOUNCE_DELAY);
            if (cur_state_g == 1) {
                cur_delay_g -= 200;
            } else {
                cur_state_g = START;
            }
        }
    }
    // Reset interrupt flag
    IFS3bits.CNAIF = 0;
}
