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

void main(void) {
    // Initialise I/O
    GPIO = 0;
    ANSEL = 0b1110100; // ADC clock derived from a dedicated internal oscillator = 500 kHz max; GP2/AN2 is analog input.
    TRISIO = 0b001100; // GP2 and GP4 are an inputs.

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

    while (1) {

        GO_nDONE = 1;   // start conversion
        while (GO_nDONE == 1) { // wait for it to complete. order of 100 us.
        }

        CCPR1L = ADRESH; // write ADC value to PWM duty cycle register

        __delay_ms(20);
    }

    return;
}
