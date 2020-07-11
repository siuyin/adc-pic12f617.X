/*
 * File:   main.c
 * Author: siuyin
 *
 * Created on 11 July, 2020, 1:20 PM
 */

// PIC12F617 Configuration Bit Settings
// 'C' source line config statements
// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/AN3/T1G/OSC2/CLKOUT, I/O function on RA5/T1CKI/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin is alternate function, MCLR function is internally disabled)
#pragma config CP = OFF         // Code Protection bit (Program memory is not code protected)
#pragma config IOSCFS = 8MHZ    // Internal Oscillator Frequency Select (8 MHz)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 8000000
#define BUTTON GP3
#define LOWER_LED GP4
#define UPPER_LED GP5

volatile unsigned char toggle_led_ctr = 0;
volatile unsigned char adc_drive_led_ctr = 0;
volatile unsigned char tick; // system timer tick is 1.024 ms

void setup_gpio(void);
void setup_adc(void);
void setup_TMR0_for_interrupts(void);
void adc_drive_led_task(void);
void toggle_led_task(void);

void main(void) {
    setup_gpio();
    setup_adc();
    setup_TMR0_for_interrupts();
    ei();

    while (1) {
        adc_drive_led_task();
        toggle_led_task();
    }

    return;
}



void setup_gpio(void) {
    // Initialise I/O
    GPIO = 0;
    ANSEL = 0b1110100; // ADC clock derived from a dedicated internal oscillator = 500 kHz max; GP2/AN2 is analog input.
    TRISIO = 0b001100; // GP2 and GP4 are an inputs.
}



void setup_adc(void) {
    // Setup ADC.
    CHS0 = 0; // Channel 2 selected for AN2.
    CHS1 = 1;
    CHS2 = 0;

    ADFM = 0; // ADC output format with most significant bits in ADRESH (AD result high).
    ADON = 1; // Turn on ADC.

    // PWM setup.
    TRISIO5 = 1; // Disable the output driver to GP5.
    P1ASEL = 1; // Select P1A*, the alternative P1A output on GP5.
    PR2 = 0x65; // set PWM period: ((PR2)+1) * 4 * Tosc * TMR2 prescaler value. ((101+1)*4*Tosc*4 = 128 us.
    P1M = 0; // PWM output on P1A
    CCP1M0 = 0;
    CCP1M1 = 0;
    CCP1M2 = 1;
    CCP1M3 = 1; // 1100 =PWM mode; P1A active-high; P1B active-high

    T2CKPS0 = 1;
    T2CKPS1 = 0; // timer 2 prescaler = 4
    TMR2IF = 0; // clear timer 2 interrupt request flag.
    TMR2ON = 1; // turn on timer 2

    while (TMR2IF == 0) { // wait for timer 2 to overflow before enabling output
    }

    TRISIO5 = 0; // enable GP5/P1A* output.
}



const unsigned char tmr0_reload_val = 246;

void setup_TMR0_for_interrupts(void) {
    tick = 0;

    // example:
    // Fosc = 1/(2RC)
    // Tcyc = 4/Fosc
    // TMR0_t = prescaler * Tcyc
    // If R=6.8k and C=100pF, and prescaler = 256, each TMR0_t = 1.39 ms
    PSA = 0; // 0: Assign prescaler to TMR0
    PS2 = 1;
    PS1 = 1;
    PS0 = 1;

    TMR0 = tmr0_reload_val; // setup for 256 - x counts before triggering interrupt.
    T0CS = 0; // 0: TMR0 counter source is internal clock

    T0IE = 1; // enable TMR0 interrupt
}



enum button_push_state_t {
    bpPushed, bpReleased, bpMaybeReleased
};

void toggle_led_task(void) {
    static enum button_push_state_t state = bpReleased;
    static unsigned char lda; // lda: last done at
    const unsigned char t = 20; // about 20 ms

    if (toggle_led_ctr != 0 || lda == tick) {
        return;
    }

    lda = tick;
    toggle_led_ctr = t;

    switch (state) {
        case bpReleased:
            if (BUTTON == 0) {
                state = bpPushed;
                LOWER_LED = ~LOWER_LED;
            }
            break;
        case bpPushed:
            if (BUTTON == 0) {
                state = bpPushed;
                break;
            }
            state = bpMaybeReleased;
            break;
        case bpMaybeReleased:
            if (BUTTON == 0) {
                state = bpPushed;
                break;
            }
            state = bpReleased;
            break;
    }
}



enum adc_conversion_state_t {
    start, complete
};

void adc_drive_led_task(void) {
    static enum adc_conversion_state_t state = start;
    static unsigned char lda; // lda: last done at
    unsigned char t = 19; // about 19 ms

    if (adc_drive_led_ctr != 0 || lda == tick) {
        return;
    }
    switch (state) {
        case start:
            GO_nDONE = 1;
            t = 1; // 1 ms is more than enough for ADC conversion.
            state = complete;
            break;
        case complete:
            CCPR1L = ADRESH; // write ADC value to PWM duty cycle register
            t = 19;
            state = start;
            break;
    }

}



// interrupt service routine

void __interrupt() interrupt_service_routine(void) {
    if (T0IE && T0IF) { // timer 0 interrupt enable and interrupt flag
        if (toggle_led_ctr > 0)toggle_led_ctr--;
        if (adc_drive_led_ctr > 0)adc_drive_led_ctr--;

        tick++;

        TMR0 = tmr0_reload_val; // reload TMR0 for next tick
        T0IF = 0; // clear TMR0 interrupt flag
    }
}