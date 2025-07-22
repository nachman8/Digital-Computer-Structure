#include "../header/api.h"
#include "../header/app.h"

// Global variables
enum FSMstate state;
enum SYSmode lpm_mode;
unsigned char stopwatch_running;  // Only defined here, not in halGPIO.c
unsigned char two_sec_counter = 0;

char str_arr[4][16] = { "Life isnt about", "finding yoursel", "Life is about  ", "creating yourse" };
/*
char str2[16] = ”finding yoursel”;
char str3[16] = ”Life is about  ”;
char str4[16] = ”creating yoursel”;
*/
void main(void) {
    // Initialize system state
    state = state0;             // Start in idle state
    lpm_mode = mode0;           // Use LPM0 for power saving
    stopwatch_running = 0;      // Stopwatch initially stopped
    
    // Configure hardware
    sysConfig();
    
    // Initialize LCD
    lcd_init();
    lcd_clear();
    
    // Main program loop - Finite State Machine implementation
    while(1) {
        switch(state) {
            case state0:  // Idle state - sleep to save power
                stop_stopwatch_timer();
                lcd_clear();
                enterLPM(lpm_mode);
                break;
                
            case state1:  // Frequency measurement
				enable_interrupts();  // Enable to Cut the state
                frequency_measurement();
                break;
                
            case state2:  // Stopwatch
				enable_interrupts();  // Enable to Cut the state
                stopwatch();
                break;
                
            case state3:  // Tone generator
                lcd_clear();
				enable_interrupts();  // Enable to Cut the state
                tone_generator();
                break;
                
            case state4:  // Unused/reserved state
                start_stopwatch_timer();
                lcd_clear();
                enterLPM(lpm_mode);
                break;

        }
    }
}
