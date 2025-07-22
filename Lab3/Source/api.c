#include "../header/api.h"       // API declarations
#include "../header/halGPIO.h"    // Hardware abstraction layer
#include "../header/app.h"        // For state/mode definitions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MIN(a, b) ((a) > (b) ? (b) : (a))

// These are declared as external in api.h, but we need to use them here
extern enum FSMstate state;
extern enum SYSmode lpm_mode;
extern int Key;
extern char idiom_recorder[32];
extern const char keyboard[16][4];
extern char data_base[12][62];
extern volatile char leds[9];
extern volatile char* leds_ptr;

/**
 * State 1: Idiom Recorder function
 * Records up to 32 characters using the keypad with multi-press input
 */
void idiom_rec(void) {
    // Display initial prompt
    disable_interrupts();
    lcd_clear();
    lcd_puts("Enter Idiom:");
    //delay(1000);
    enterLPM(mode0);

    unsigned int i = 0;  // Character index counter

    // Main input loop - continue until state changes (another button pressed)
    while (state == state1) {
        unsigned int keyInput;    // Currently pressed key
        unsigned int pressCount = 0;  // Count of consecutive presses of the same key

        // Wait for initial key press
        Key = 16; // Reset key value
        en_keypad_interrupts();
        enterLPM(mode0);
        disable_keypad_interrupts();

        // If state has changed, exit (another pushbutton was pressed)
        if (state != state1) {
            break;
        }

        keyInput = Key;  // Store pressed key

        // Check if we've reached maximum character count
        if (i >= 32) {
            // Display message and wait for state change
            lcd_clear();
            lcd_puts("Max characters reached. Press other button.");
            continue;
        }

        // Multi-press detection loop
        while (Key < 16 && Key == keyInput) {
            // Exit loop if we've reached max characters for this key
            if ((keyInput < 12 && pressCount >= 1) || (keyInput >= 12 && pressCount >= 2)) {
                break;
            }

            // Reset key and wait for possible additional press
            Key = 16;
            en_keypad_interrupts();
            startTimerA();  // Start timer for multi-press timeout
            enterLPM(mode0);
            finishTimerA();
            disable_keypad_interrupts();

            // Check if state changed during wait
            if (state != state1) {
                break;
            }

            // If same key pressed again during timeout, increment press count
            if (Key == keyInput) {
                pressCount++;
            }
        }

        // Exit if state changed
        if (state != state1) {
            break;
        }

        // Process valid key press
        if (keyInput < 16 && pressCount < 3) {
            // Clear LCD for first character, or move to next line after 16 chars
            if (i == 0) {
                lcd_clear();
            } else if (i == 16) {
                lcd_new_line;
            }

            // Get character from keyboard matrix and display it
            idiom_recorder[i] = keyboard[keyInput][pressCount];
            lcd_data(idiom_recorder[i]);

            i++;  // Move to next character position
        }
    }
    
    // Ensure timer is stopped when exiting
    finishTimerA();

    // Add null terminator to string
    if (i < 32) {
        idiom_recorder[i] = '\0';
    } else {
        idiom_recorder[31] = '\0';
    }
}

/**
 * Helper function for strMirror that performs the actual DMA mirroring
 * @param data Source data array
 * @param idx Index of phrase to mirror
 * @param strout Output buffer for mirrored string
 * @return Pointer to the mirrored string
 */
char* dma_strMirror(char** data, int idx, char* strout) {
    // DMA initializations
    if (idx == -1){
        strout = "debug print - idx == -1";
        return strout;
    }
    int len = strlen(data[idx]);

    // Clear output buffer
    memset(strout, 0, 80);

    //set src / dst addresses
    DMA0_Src_Add = (unsigned int)(data[idx] + len - 1);
    DMA0_Dst_Add = (unsigned int)strout;
    // Set transfer size
    DMA0_Size = len;



    // Enable and trigger DMA
    triggerDMA();

    // Wait for DMA to complete
    while(DMA0CTL & DMAEN);
    // Ensure null termination
    strout[len] = '\0';

    // Stop DMA
    stopDMA();

    return strout;
}

/**
 * State 2: String Mirror function
 * Mirrors a selected phrase from the data_base using DMA
 * User selects a phrase index (0≤i<11) with the keypad
 * Mirrors that phrase and displays the result on LCD
 */
void strMirror(void) {
    int strIndex = -1;      // Selected string index
    char strout[80];        // Buffer for mirrored string
    char sliced[33];
    int i, max_idx;
    // Create an array of pointers to each row of data_base
    char* data_ptrs[12];
    for (i = 0; i < 12; i++)  {
        data_ptrs[i] = data_base[i];

    }
    //variables for tracking lcd print counts
    int strLen;
    int displayed=0;
    unsigned int keyInput;
    //config DMA0CTL for mirror usage
    startDMA();
    // Initial prompt
    lcd_clear();
    lcd_puts("Select string (0-10) and press *");//

    // Main operation loop
    while (state == state2) {


        // Wait for keypad input
        Key = 16; // Reset key value
        en_keypad_interrupts();
        enterLPM(mode0);
        disable_keypad_interrupts();

        // Check if state changed
        if (state != state2) {
            break;
        }

        keyInput = Key;
        Key = 16;  // Reset key



        // Process key input for string selection (0-10 only)
        if (keyInput <= 10 && displayed == 0) {
            // Valid string selection - display it
            strIndex = keyInput;
            lcd_clear();
            dma_strMirror(data_ptrs, strIndex, strout);
            strncpy(sliced, &strout[0], 32);
            sliced[32] = '\0';  // Ensure null termination
            lcd_puts(sliced);
            displayed = 32;
            //lcd_puts(data_base[keyInput]);

        }
        ////////////////////////////////////////////////////////////////////
        else if (keyInput == 12 && strIndex != -1 && displayed > 0) {  // * key pressed and string selected

            // Display the mirrored string
            //lcd_clear();
            //lcd_puts(strout);

            // Handle long strings with scrolling (two rows with scrolling)
            strLen = strlen(strout);

            if (strLen > 32) { // If the string is longer than can fit on screen
                while (displayed < strLen && state == state2) {
                    // Check if state changed
                    if (state != state2) {
                        break;
                    }
                    // Check if * pressed for scrolling
                    if (keyInput == 12) {
                        lcd_clear();
                        dma_strMirror(data_ptrs, strIndex, strout);
                        max_idx = MIN(32,strLen-displayed);
                        strncpy(sliced, &strout[displayed], max_idx+1);
                        sliced[32] = '\0';  // Ensure null termination
                        // Display next screen
                        lcd_puts(sliced);
                        displayed += max_idx;

                        // Prompt to continue
                        Key = 16;
                        en_keypad_interrupts();
                        enterLPM(mode0);
                        disable_keypad_interrupts();
                        keyInput = Key;
                    }

                    // Reset key
                    Key = 16;
                } //end while displayed < strLen
            } // end if strLen > 32

            // Return to idle state
            state = state0;
            break;
        }// end elsif key==12
    }// end while state==state2
}

/**
 * State 3: LED pattern display using DMA
 * Cycles through LED patterns using DMA with block transfer mode
 * LED values: {128,224,32,126,83,44,253,113,160}
 * Uses TimerB for 500ms transitions
 */
void shift_leds(void) {
    unsigned int i = 0;

    // Reset LED pointer to the first element (128)
    leds_ptr = &leds[0];
    lcd_puts("printing to LEDs");
    // Run through the LED patterns 8 times (9 elements)
    for (i = 0; i < 9; i++) {
        // Exit if state has changed
        if (state != state3) {
            break;
        }

        startDMA();
        // Set LED value directly
        DMA0_Src_Add = (unsigned int)leds_ptr;
        DMA0_Dst_Add = (unsigned int)&LEDsArrPort;
        DMA0_Size = 0x0001;
        triggerDMA();
        startTimerB();

        // Wait for timer interrupt (500ms)
        enterLPM(mode0);
        finishTimerB();
        leds_ptr++;                     //increment pointer
        if (leds_ptr == &leds[9]){      //check if pointer is at the end of the array
            leds_ptr = &leds[0];         //reset pointer to the beginning of the array
        }
        // Stop timer
    }
    stopDMA();
    // Clean up and return to idle
    clear_LEDs();
    state = state0;
}

/**
 * State 4: String Merge function
 * Merges two strings (str1 and str2) using DMA
 */
void merge(void) {
    char strMerge[80];        // Buffer for merged string
    char sliced[33];          // Buffer for display (32 chars + null)
    int i, max_idx;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int strLen;
    int displayed = 0;
    unsigned int keyInput;

    // Clear display and show initial message
    lcd_clear();
    lcd_puts("debug print - Merging strings...");

    // Clear output buffer
    memset(strMerge, 0, 80);

    // Configure DMA for string merging
    startDMA();

    // Set up DMA0 (higher priority, for first string)
    DMA0_Src_Add = (unsigned int)str1;
    DMA0_Dst_Add = (unsigned int)strMerge;
    DMA0_Size = len1;

    // Set up DMA1 (for second string)
    DMA1_Src_Add = (unsigned int)str2;
    DMA1_Dst_Add = (unsigned int)(strMerge + len1);  // Start at the end of first string
    DMA1_Size = len2;

    // Start DMA0 transfer (higher priority)
    triggerDMA0();

    // Enter low power mode, wait for DMA0 interrupt
    //enterLPM(mode0);
    // Wait for DMA to complete
    while(DMA0CTL & DMAEN);

    // After DMA0 completes and wakes us up, start DMA1 transfer
    triggerDMA1();
    // Wait for DMA to complete
    while(DMA1CTL & DMAEN);
    // Enter low power mode, wait for DMA1 interrupt
    enterLPM(mode0);

    // Ensure null termination
    strMerge[len1 + len2] = '\0';

    // Get total string length
    strLen = len1 + len2;

    // Display the first 32 characters initially
    lcd_clear();
    strncpy(sliced, &strMerge[0], 32);
    sliced[32] = '\0';  // Ensure null termination
    lcd_puts(sliced);
    displayed = 32;

    // Main operation loop for displaying string with pagination
    while (state == state4) {
        // Wait for keypad input
        Key = 16; // Reset key value
        en_keypad_interrupts();
        enterLPM(mode0);
        disable_keypad_interrupts();

        // Check if state changed
        if (state != state4) {
            break;
        }

        keyInput = Key;
        Key = 16;  // Reset key

        // Check if * key pressed for scrolling
        if (keyInput == 12) {  // * key
            // Handle long strings with scrolling
            if (strLen > displayed) { // If there are more characters to display
                lcd_clear();
                max_idx = MIN(32, strLen - displayed);
                strncpy(sliced, &strMerge[displayed], max_idx);
                sliced[max_idx] = '\0';  // Ensure null termination

                // Display next screen
                lcd_puts(sliced);
                displayed += max_idx;
            } else {
                // No more characters to display, return to idle state
                state = state0;
                break;
            }
        }
    }

    // Return to idle state if we exit the loop
    state = state0;
}
