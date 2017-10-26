#include "morse_code.h"      

void morse_code_encoding(uint8_t msg[], uint8_t *res) 
{
    uint8_t *morse_table[] = {
        "13","3111","3131","311","1","1131","331","1111","11","1333",
        "313","1311","33","31","333","1331","3313","131","111",
        "3","113","1113","133","3113","3133","3311"
    };
    for (uint32_t i = 0; msg[i]!='\0'; i++) {
        if (' ' == msg[i]) {
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
    ANSELG &= ~((1 << 6) | (1 << 15));
    ANSELBbits.ANSB11 = 0;
    // Set direction to output 
    TRISG &= ~((1 << 6) | (1 << 15));
    TRISBbits.TRISB11 = 0;
    TRISDbits.TRISD4 = 0;
    // Turn off LEDs for initialization
    LED_1 = LED_2 = LED_3 = LED_4 = 0;
    // BTN1 and BTN2 inputs
    // Disable analog mode
    ANSELAbits.ANSA5 = 0;
    // Set directions to input
    TRISA |= (1 << 5) | (1 << 4);
    // 3. Configure peripheral to generate interrupts
    // Enable change notification interrupt in CN
    CNENA |= (1 << 5) | (1 << 4);
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
    uint32_t delay_mult;
    while (val --> 0) {
    	for(delay_mult = 10000; delay_mult > 0; delay_mult--);
    }
}

void blink_leds(uint32_t mode) 
{
    if (1 == mode) {
        LED_1 = 1;
        delay(cur_delay_g);
        while(PAUSE == cur_state_g);
        LED_1 = 0;
        delay(cur_delay_g);
    } else if (3 == mode) {
        LED_2 = LED_3 = LED_4 = 1;
        delay(cur_delay_g);
        while(PAUSE == cur_state_g);
        LED_2 = LED_3 = LED_4 = 0;
        delay(cur_delay_g);
    }
}

void display_msg(void) 
{
    cur_delay_g = BLINK_DELAY;
    if (START == cur_state_g) { 
        for(uint32_t i = 0; encoded_msg_g[i]!='\0'; i++) {
            if (RESET == cur_state_g) {
                break;
            }
            if ('1' == encoded_msg_g[i]) {
                blink_leds(1);
            } else if ('3' == encoded_msg_g[i]) {
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
    if (CNSTATAbits.CNSTATA4) { // Bit 4 (BTN2) changed
        if (BTN_2)  { 
            delay(DEBOUNCE_DELAY);
            cur_state_g = PAUSE;
        }
    }
    if (CNSTATAbits.CNSTATA5) { // Bit 5 (BTN1) changed
        if (BTN_1)  { 
            delay(DEBOUNCE_DELAY);
            if (START == cur_state_g) {
                cur_delay_g -= 200;
            } else {
                cur_state_g = START;
            }
        }
    }
    // Reset interrupt flag
    IFS3bits.CNAIF = 0;
}
