# ADC, Timers and Comparator on PIC12F167

# Setup
- GP3 button input with pull-up resistor
- GP4 positive logic output driving lower LED
- GP5 positive logic output driving upper LED
- GP2/AN2 analog input connected to potentiometer

# ADC: Analog to Digital Converter
The analog to digital converter is configured to use
an internal RC oscillator with an ADC conversion
time (Tad) of 2 to 6 microseconds per bit. A 10-bit conversion
requires 11 Tad or a maximum of 66 us. Typical Tad is 4 us for
a conversion time of 44 us.
