#ifndef _api_H_
#define _api_H_

#include "../header/halGPIO.h"     // Hardware Abstraction Layer

// State 1: Frequency Measurement functions
extern void frequency_measurement(void);
extern void display_frequency(unsigned int freq);

// State 2: Stopwatch functions
extern void stopwatch(void);
extern void format_time(unsigned int seconds, char* time_str);
extern void start_stopwatch_timer(void);
extern void stop_stopwatch_timer(void);
// State 3: Tone Generator functions
extern void tone_generator(void);
extern unsigned int calculate_pwm_period(unsigned int adc_value);

#endif
