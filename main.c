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


#define BUTTON GP3
#define LOWER_LED GP4
#define UPPER_LED GP5

void main(void) {
    // Initialise I/O
    GPIO = 0;
    ANSEL = 0b1110100; // ADC clock derived from a dedicated internal oscillator = 500 kHz max; GP2/AN2 is analog input.
    TRISIO = 0b001100; // GP2 and GP4 are an inputs.

    while (1) {
        LOWER_LED = ~BUTTON;
        UPPER_LED = BUTTON;
    }

    return;
}
