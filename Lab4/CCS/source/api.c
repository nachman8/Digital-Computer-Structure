#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdlib.h>
#include "stdio.h"
#include <string.h>
#include "../header/iqmath_msp430.h"

//-------------------------------------------------------------
//                    convert int to string
//-------------------------------------------------------------
void to_string(char *str, unsigned int num){        // convert integer to string
    long tmp = num;
	int len = 0 , j;                     // tmp is a temporary variable to store the number
	
    while(tmp){                            // calculate the length of the number
        len++;
        tmp /= 10;
    }
    for(j = len - 1; j >= 0; j--){      
        str[j] = (num % 10) + '0';       // get the last digit of the number to ascii
        num /= 10;                    
    }
    str[len] = '\0';                // add null character at the end of the string
}

//-------------------------------------------------------------
//                       Count up to LCD (State 1)
//-------------------------------------------------------------
void count(){
    char temp[6];
    
    while(state == state1){
        to_string(temp, num++);     // convert num to string and store it in temp
        if (num >= (1U << 13)-1) num = 0; // reset num to 0 if it reaches 2^13-1
        lcd_home();                 // move the cursor to the first line
        lcd_puts(temp);             // display the number on the LCD
        startTimerA0();            // start timer A0
        enterLPM(mode0);       // enter low power mode
        finishTimerA0();       // finish timer A0
    }
}

//-------------------------------------------------------------
//                         Buzzer (State 2)
//-------------------------------------------------------------
void buzzer(){
    //tone series = {1kHz, 1.25kHz, 1.5kHz, 1.75kHz, 2kHz, 2.25kHz, 2.5kHz}
    // CCR value calculation = f_SMCLK / tone_series[j] = ((1 << 20)/tone_series[j])
    // TimerA1 - PWM signal for the buzzer, TimerA0 - delay
    unsigned int tone_series[7] = {1000, 1250, 1500, 1750, 2000, 2250, 2500};   // Given tone series
    unsigned int CCR_values[7];                                         // CCR values for the tone series
     unsigned int j = 0;
    for (j = 0; j < 7; j++){
        CCR_values[j] = (unsigned int)((1UL << 20) / tone_series[j]);         // convert the tone series to CCR values
    }
    unsigned int i = 0;
    startTimerA1();                     // TimerA1 generates PWM signal for the buzzer
    enableTimerA1_bzr();                // Configure TimerA1 for buzzer (Reset/set mode for PWM with 50% duty cycle)
    while(state == state2){
        Timer1_CCR0 = CCR_values[i];          // Set value - set the frequency of the buzzer
        Timer1_CCR2 = CCR_values[i++]>>1;    // Reset value - set the duty cycle of the buzzer to 50% and increment i
        if(i == 7) i = 0;                          // reset i to 0 if it reaches 7
        startTimerA0();                             // start timer A0 - delay
        enterLPM(mode0);                            // enter low power mode
        finishTimerA0();                            // finish timer A0
    }
    disableTimerA1_bzr();
    finishTimerA1();
}

//-------------------------------------------------------------
//                         Set X value (State 3)
//-------------------------------------------------------------
// Each timer tick is 8/2^20 = 0.007629 ms. We use up/down mode, so the delay is 2 times the value of CCR0
// define real_time_tick = (8/2^20) * 2 = 2^-16 = 0.015625 ms (include the time for the timer to count up and down)
// CCR0 = (X * 10^-3)/ real_time_tick = X * 2^16 * 10^-3 = X * 65.536 - 1
// Add these debug variables at the top of your file (global scope)
// Add these debug variables at the top of your file (global scope)
void set_X(){
    finishTimerA0();
    unsigned int input_X = atoi(timerInput);

    // X * 65.536 = X * 65536 / 1000
    // Using integer arithmetic only
    unsigned long temp = ((unsigned long)input_X * 65536UL) / 1000UL;

    if (temp > 0) {
        X = (unsigned int)(temp - 1);
    } else {
        X = 0;
    }

    if (X >= 0xFFFF){
        X = 0xFFFF;
    }
    X_flag = 0;
    TIMER0_A0_config();
}

//-------------------------------------------------------------
//                      Measure POT Value (State 4)
//-------------------------------------------------------------

void measPOT(){
    _iq16 POT_meas_iq;
    _iq16 reference_voltage = _IQ16(3.6);    // 3.6V reference voltage in Q16
    _iq16 adc_max = _IQ16(1023);             // Maximum ADC value in Q16
    int voltage_int, voltage_frac1, voltage_frac2, voltage_frac3;

    while(state == state4){
        startADC10();
        enterLPM(mode0);
        finishADC10();

        // Convert ADC value to Q16 voltage: voltage = (3.6 * adc_value) / 1023
        _iq16 adc_value_iq = _IQ16(get_ADC_value());  // Convert ADC reading to Q16
        // Better: multiply by larger number first
        POT_meas_iq = _IQmpy(_IQdiv(reference_voltage, adc_max), adc_value_iq);
        // Extract integer part
        voltage_int = _IQint(POT_meas_iq);

        // Extract fractional digits more carefully
        _iq16 fractional_part = POT_meas_iq - _IQ16(voltage_int); // Get fractional part
        voltage_frac1 = _IQint(_IQmpy(fractional_part, _IQ16(10)));      // First decimal
        voltage_frac2 = _IQint(_IQmpy(fractional_part, _IQ16(100))) % 10; // Second decimal
        voltage_frac3 = _IQint(_IQmpy(fractional_part, _IQ16(1000))) % 10; // Third decimal

        lcd_home();
        lcd_puts("Potentiomter");
        lcd_new_line;
        lcd_puts("Meas: ");
        lcd_putchar(voltage_int + '0');                     // Integer part
        lcd_putchar('.');
        lcd_putchar(voltage_frac1 + '0');                   // First decimal digit
        lcd_putchar(voltage_frac2 + '0');                   // Second decimal digit
        lcd_putchar(voltage_frac3 + '0');                   // Third decimal digit
        lcd_puts(" [v]");

        startTimerA0();
        enterLPM(mode0);
        finishTimerA0();
    }
}




//-------------------------------------------------------------
//                      Send Message on PB1 Press (State 5)
//-------------------------------------------------------------
void print_string(){
    // Wait for PB1 press in this state
    // The actual message transmission is handled by PB1 interrupt and TX ISR
    int len = strlen(sentence ) ;
    while(state == state5){
        enable_UARTRX_interrupts();
        enterLPM(mode0);  // Wait for PB1 press or state change
	    while(k < len && state == state5) {
			//UCA0TXBUF = sentence[k++];
			enable_UARTTX_interrupts();
		}
		print_negev_flag = 0;
    }
}



// Solution 1: Add a global variable to store results
volatile int test_result = 0;  // Add this as a global variable

void test_qformat(){
    _iq16 test_a = _IQ16(3.6);
    _iq16 test_b = _IQ16(512);  // Half of 1024 ADC range
    _iq16 test_c = _IQ16(1023);
    _iq16 result = _IQdiv(_IQmpy(test_a, test_b), test_c);
    // result should be approximately 1.8V (half of 3.6V)
    int voltage_int = _IQint(result);  // Should be 1

    // Store result to prevent optimization
    test_result = voltage_int;

    // Optional: Add a simple verification
    if (voltage_int == 1) {
        test_result = 1;  // Test passed
    } else {
        test_result = -1; // Test failed
    }
}




//-------------------------------------------------------------
//                      Display ASCII Code (State 7)
//-------------------------------------------------------------
void display_ascii(){
    while(state == state7){
        enable_UARTRX_interrupts();
        enterLPM(mode0);  // Wait for character input

        if(ascii_display_flag && state == state7){
            char ascii_str[4];
            unsigned int ascii_value = (unsigned int)ascii_char;
            char lower, upper;
            /*lower = ascii_char & 0x0F + '0';
            upper = (ascii_char >> 4) & 0x0F + '0';
            lcd_puts(upper);
            lcd_puts(lower);*/



            // Convert ASCII value to string
            ascii_str[0] = (ascii_value / 100) + '0';        // Hundreds
            ascii_str[1] = ((ascii_value / 10) % 10) + '0';  // Tens
            ascii_str[2] = (ascii_value % 10) + '0';         // Units
            ascii_str[3] = '\0';                             // Null terminator

            // Display on LCD
            lcd_home();

            lcd_puts(ascii_str);


        }

    }
    ascii_display_flag = 0;  // Reset flag
    lcd_clear();
}
