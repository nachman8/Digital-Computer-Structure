#ifndef _app_H_
#define _app_H_

// FSM states enumeration
enum FSMstate {
    state0,     // Idle state (sleep mode)
    state1,     // Frequency measurement
    state2,     // Stopwatch
    state3,     // Tone generator
    state4      // Reserved for future use
};

// System operation modes enumeration
enum SYSmode {
    mode0,      // LPM0
    mode1,      // LPM1
    mode2,      // LPM2
    mode3,      // LPM3
    mode4       // LPM4
};

// Global variables declaration
extern enum FSMstate state;       // Current FSM state
extern enum SYSmode lpm_mode;     // Current low-power mode
extern unsigned char stopwatch_running;  // Flag for stopwatch run/pause
extern unsigned char two_sec_counter;    // flag for realtime task (state4)
extern char str_arr[4][16]; // Use the array defined in app.h


#endif
