#ifndef _api_H_
#define _api_H_

#include "../header/app.h"    // Include app layer definitions
#include "../header/halGPIO.h" // Include HAL layer definitions

// External variables declarations
extern enum FSMstate state;   // Current system state
extern enum SYSmode lpm_mode; // Current low power mode
extern int Key;               // Current key pressed
extern char idiom_recorder[32]; // Buffer for idiom recording
extern const char keyboard[16][4]; // Keypad character mapping
extern char data_base[12][62];     // Database of idioms/phrases
extern volatile char leds[9];               // LED pattern array
extern volatile char* leds_ptr;             // Pointer to current LED pattern

/**
 * API Function Declarations
 */

/**
 * State 1: Idiom Recorder function
 * Records up to 32 characters using the keypad with multi-press input
 * Uses keypad interrupts and timer for multi-press detection
 */
void idiom_rec(void);

/**
 * State 2: String Mirror function
 * Mirrors a selected phrase from the data_base using DMA
 * User selects a phrase index (0≤i<11) with the keypad
 * Mirrors that phrase and displays the result on LCD
 */
void strMirror(void);

/**
 * Helper function for strMirror that performs the actual DMA mirroring
 * @param data Source data array (cast to char**)
 * @param idx Index of phrase to mirror
 * @param strout Output buffer for mirrored string
 * @return Pointer to the mirrored string
 */
char* dma_strMirror(char** data, int idx, char* strout);

/**
 * State 3: LED pattern display using DMA
 * Cycles through LED patterns using DMA with block transfer mode
 * LED values: {128,224,32,126,83,44,253,113,160}
 * Uses TimerB for 500ms transitions
 */
void shift_leds(void);


/**
 * State 4: String Merge function
 * Merges two strings (str1 and str2) using DMA
 * Uses two DMA channels sequentially in block transfer mode
 * Displays the result on LCD
 */
void merge(void);

// External declarations for the strings
extern char str1[];
extern char str2[];

#endif // _api_H_
