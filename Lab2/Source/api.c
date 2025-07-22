#include "../header/api.h"
#include <stdio.h>

//-------------------------------------------------------------
//              Frequency Measurement (State 1)
//-------------------------------------------------------------
/*void frequency_measurement(void) {
    WDTCTL = WDTPW + WDTHOLD;  // Stop watchdog timer
    float N_SMCLK;
    float freq;
    float tmp = 0;
    float SMCLK_FREQ = 1048576;   // 2^20
    unsigned int real_freq;
    char strFreq[6];
    
    // Stop and reset timer first
    TA1CTL = MC_0 + TACLR;
    
    // Initialize and start timer fresh
    TA1CTL = TASSEL_2 + MC_2 + TACLR;  // SMCLK, continuous mode, clear
    
    // Make sure CCR2 is configured correctly
    TA1CCTL2 = CAP + CM_1 + SCS + CCIS_0;  // Capture mode, rising edge, synchronize
    
    // Show initial LCD template
    write_freq_tmp_LCD();
    
    while(state == state1) {
        // Reset capture variables
        REdge1 = 0;
        REdge2 = 0;
        
        // Clear string buffer
        unsigned int i;
        for(i = 0; i < 6; i++) {
            strFreq[i] = '\0';
        }
        
        // Enable capture interrupt
        TA1CCTL2 |= CCIE;
        
        // Wait for two rising edges to be captured
        __bis_SR_register(LPM0_bits + GIE);
        
        // Check if we have valid captures
        if(REdge1 == 0 || REdge2 == 0) {
            continue;  // Skip if capture failed
        }
            
        // Calculate frequency with calibration factor
        tmp = 0.985;
        N_SMCLK = 0.9 * (REdge2 - REdge1) * tmp;
        freq = SMCLK_FREQ / N_SMCLK;
        real_freq = (unsigned int)freq;
        
        // Skip invalid readings
        if(real_freq < 50 || real_freq > 20000) {
            continue;
        }
            
        // Convert to string and display
        sprintf(strFreq, "%d", real_freq);
        
        // Update LCD with frequency
        write_freq_tmp_LCD();  // Refresh template
        lcd_home();            // Go to start of line
        lcd_cursor_right();    // Move to position after "fin="
        lcd_cursor_right();
        lcd_cursor_right();
        lcd_cursor_right();
        lcd_puts(strFreq);     // Display frequency
        
        cursor_off;            // Turn off cursor
        DelayMs(500);          // Wait before next measurement
    }
    
    // Stop timer when exiting
    TA1CTL = MC_0;
}*/
void frequency_measurement(void) {
    WDTCTL = WDTPW + WDTHOLD;                  // Stop watchdog timer

    // Variables
    unsigned int real_freq;
    char freq_buffer[6] = {0};
    const float EPSILON = 0.02;                // 2% change threshold
    unsigned long old_cycles = 0;              // Previous cycle count

    // Setup LCD template once - direct approach
    lcd_clear();
    lcd_home();
    lcd_puts("fin =       Hz");
    cursor_off;

    // Configure timer
    TA1CTL = TASSEL_2 + MC_2 + TACLR;          // SMCLK, continuous mode, clear
    TA1CCTL2 = CAP + CM_1 + SCS + CCIS_0;      // Capture mode, rising edge

    while(state == state1) {
        // Reset capture variables
        REdge1 = 0;
        REdge2 = 0;

        // Wait for capture
        TA1CCTL2 |= CCIE;
        __bis_SR_register(LPM0_bits + GIE);

        // Exit if state changed
        if(state != state1) break;

        // Skip invalid captures
        if(REdge1 == 0 || REdge2 == 0) continue;

        // Calculate cycles
        unsigned long cycles;
        if(REdge2 > REdge1) {
            cycles = REdge2 - REdge1;
        } else {
            cycles = (0xFFFF - REdge1) + REdge2;
        }

        // Calculate frequency - using precise fixed factor
        real_freq = (unsigned int)((1185000UL) / cycles);

        // Ensure we only process frequencies in the required 50Hz-20kHz range
        if(real_freq >= 50 && real_freq <= 20000) {
            // Only update if it's the first reading or if the change is significant
            if(old_cycles == 0 ||
               ((cycles > old_cycles) ?
                (cycles - old_cycles) : (old_cycles - cycles)) > old_cycles * EPSILON) {

                // Convert to string using simple %d format (works with minimal printf)
                sprintf(freq_buffer, "%d", real_freq);

                // Update display - simple and direct approach that reduces flickering
                lcd_goto(6);        // Position after "fin = "
                lcd_puts(freq_buffer);

                // Give the LCD time to stabilize
                __no_operation();
                __no_operation();

                // Store current cycles for future comparison
                old_cycles = cycles;
            }
        }
    }

    // Stop timer when exiting
    TA1CTL = MC_0;
}
//-------------------------------------------------------------
//              Stopwatch (State 2)
//-------------------------------------------------------------
void stopwatch(void) {
    // Initialize LCD for stopwatch display
    lcd_clear();
    lcd_home();

    // Reset global state variables
    seconds = 0;
    stopwatch_running = 0;

    // Format and display initial time (00:00)
    char time_str[6] = "00:00";
    lcd_puts(time_str);

    // Configure and enable Timer A0 for 1-second intervals
    start_stopwatch_timer();

    // Main stopwatch loop - keep it simple!
    while (state == state2) {
        // Just wait for timer interrupts and state changes
        __bis_SR_register(LPM0_bits + GIE);
        
        // Exit if state has changed
        if (state != state2) break;
    }
    
    // Stop timer when exiting
    TA0CTL &= ~MC_3;
    TA0CCTL0 &= ~CCIE;
    stopwatch_running = 0;
}

void stop_stopwatch_timer(void) {
    TA0CTL &= ~MC_3;
    TA0CCTL0 &= ~CCIE;
    stopwatch_running = 0;
}
//-------------------------------------------------------------
//              Start Stopwatch Timer
//-------------------------------------------------------------
void start_stopwatch_timer(void) {
    TA0CTL = TASSEL_2 + MC_3 + ID_3 + TACLR;  // SMCLK, Up/Down mode, /8 divider, clear
    TA0CCR0 = 62500;                          // For 1-second interval with 1MHz/8 clock
    TA0CCTL0 = CCIE;                          // Enable CCR0 interrupt
}

//-------------------------------------------------------------
//              Format Time String
//-------------------------------------------------------------
void format_time(unsigned int sec, char* time_str) {
    unsigned int minutes = sec / 60;
    unsigned int seconds = sec % 60;
    
    // Format as MM:SS
    time_str[0] = (minutes / 10) + '0';
    time_str[1] = (minutes % 10) + '0';
    time_str[2] = ':';
    time_str[3] = (seconds / 10) + '0';
    time_str[4] = (seconds % 10) + '0';
    time_str[5] = '\0';
}

//-------------------------------------------------------------
//              Tone Generator (State 3)
//-------------------------------------------------------------
/*void tone_generator(void) {
    // Configure Timer A1 for PWM output
    TA1CTL = TASSEL_2 + MC_1 + TACLR;  // SMCLK, up mode, clear
    TA1CCTL1 = OUTMOD_7;               // Reset/Set mode for PWM
    
    // Initialize LCD
    lcd_clear();
    lcd_home();
    lcd_puts("Tone: ");
    
    while (state == state3) {
        // Start ADC conversion
        ADC10CTL0 |= ENC + ADC10SC;             // Start sampling
        __bis_SR_register(LPM0_bits + GIE);     // Wait for conversion
        
        if (state != state3) break;             // Check if state changed
        
        // Get ADC result and map to frequency range
        unsigned int adc_value = ADC10MEM;
        
        // Linear mapping from ADC (0-1023) to frequency (1000-2500 Hz)
        // f_out = 1000 + 1500 * (adc_value / 1023)
        // Using fixed-point math instead of floating point
        unsigned int frequency = 1000 + ((unsigned long)adc_value * 1500) / 1023;
        
        // Calculate timer period for PWM
        unsigned int period = 1000000 / frequency;  // Period = SMCLK/frequency
        TA1CCR0 = period;                           // Set PWM period
        TA1CCR1 = period / 2;                       // 50% duty cycle
        
        // Display the frequency on LCD
        char freq_str[10];
        sprintf(freq_str, "%d Hz", frequency);
        lcd_goto(6);                               // Position after "Tone: "
        lcd_puts(freq_str);
        
        // Small delay to limit update rate
        DelayMs(100);
    }
    
    // Stop PWM when exiting
    TA1CTL &= ~MC_1;
}
*/
void tone_generator(void) {
    // Configure Timer A1 for PWM output
    TA1CTL = TASSEL_2 + MC_1 + TACLR;  // SMCLK, up mode, clear
    TA1CCTL1 = OUTMOD_7;               // Reset/Set mode for PWM

    // Calibrated constants for the m*NADC + n equation
    const int n = 1228;               // For exactly 1kHz (slightly adjusted)
    const long m_num = -805;          // Adjusted for precisely 2.5kHz
    const long m_den = 1023;          // Max ADC value

    // Simple filtering
    unsigned int adc_filtered = 0;

    while (state == state3) {
        // Start ADC conversion
        ADC10CTL0 |= ENC + ADC10SC;
        __bis_SR_register(LPM0_bits + GIE);

        if (state != state3) break;

        // Simple low-pass filter (3/4 old + 1/4 new)
        adc_filtered = (adc_filtered * 3 + ADC10MEM) / 4;

        // Apply the MCU equation: Nfout = m*NADC + n
        long timer_period = n + ((m_num * (long)adc_filtered) / m_den);

        // Set PWM values
        TA1CCR0 = (unsigned int)timer_period;
        TA1CCR1 = (unsigned int)(timer_period / 2);  // 50% duty cycle
    }

    // Stop PWM when exiting
    TA1CTL &= ~MC_1;
}
//-------------------------------------------------------------
//              Calculate PWM Period for Frequency
//-------------------------------------------------------------
unsigned int calculate_pwm_period(unsigned int freq) {
    // Calculate timer period value for desired frequency
    // Period = SMCLK / frequency
    return 1000000 / freq;  // Assuming 1MHz SMCLK
}
