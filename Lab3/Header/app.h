#ifndef _app_H_
#define _app_H_

/**
 * Finite State Machine states for the application
 * state0: Idle state - waiting for button press
 * state1: Idiom recording using keypad
 * state2: Word operations using DMA
 * state3: LED shifting using DMA
 * state4: Return to idle
 */
enum FSMstate {
    state0,  // Idle state
    state1,  // Idiom recorder
    state2,  // Word operations
    state3,  // LED shifting
    state4   // Return to idle
};

/**
 * System low power modes
 * mode0: LPM0 - CPU and MCLK are disabled, SMCLK and ACLK remain active
 * mode1: LPM1 - CPU and MCLK are disabled, DCO is disabled if not used by SMCLK, ACLK remains active
 * mode2: LPM2 - CPU, MCLK, DCO are disabled, SMCLK is disabled, ACLK remains active
 * mode3: LPM3 - CPU, MCLK, DCO, SMCLK are disabled, ACLK remains active
 * mode4: LPM4 - CPU and all clocks are disabled
 */
enum SYSmode {
    mode0,  // LPM0
    mode1,  // LPM1
    mode2,  // LPM2
    mode3,  // LPM3
    mode4   // LPM4
};

//extern int Key;                  // Current key press (16 = no key pressed)
//extern char idiom_recorder[32];       // Buffer for idiom recording (state 1)
//extern volatile char leds[9];
//extern volatile char* leds_ptr;
#endif
