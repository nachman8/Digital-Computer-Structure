#include  "../header/api.h"    // private library - API layer
#include  "../header/app.h"    // private library - APP layer

enum FSMstate state;           // Current state of the finite state machine
enum SYSmode lpm_mode;         // Current low-power mode
int Key = 16;                  // Current key press (16 = no key pressed)
char idiom_recorder[32];       // Buffer for idiom recording (state 1)

// Keypad character mapping (0-15 keys, each with up to 4 possible characters)
const char keyboard[16][4] = {
    {'1','G','\0','\0'},    // Key 0
    {'2','H','\0','\0'},    // Key 1
    {'3','I','\0','\0'},    // Key 2
    {'C','J','\0','\0'},    // Key 3
    {'4','K','\0','\0'},    // Key 4
    {'5','L','\0','\0'},    // Key 5
    {'6','M','\0','\0'},    // Key 6
    {'D','N','\0','\0'},    // Key 7
    {'7','O','\0','\0'},    // Key 8
    {'8','P','\0','\0'},    // Key 9
    {'9','Q','\0','\0'},    // Key 10
    {'E','R','\0','\0'},    // Key 11
    {'*','S','T','\0'},     // Key 12 (*)
    {'0','U','V','\0'},     // Key 13
    {'#','W','X','\0'},     // Key 14 (#)
    {'F','Y','Z','\0'}      // Key 15
};



// Collection of idioms/phrases for state 2
char data_base[12][62] = {
    "An apple a day keeps the doctor away",
    "Give someone the benefit of the doubt",
    "Time flies when you're having fun",
    "He who pays the piper calls the tune",
    "The pen is mightier than the sword",
    "The pot calling the kettle black",
    "Wrap your head around something",
    "We'll cross that bridge when we come to it",
    "A bird in the hand is worth two in the bush",
    "Do unto others as you would have them do unto you",
    "You can catch more flies with honey than you can with vinegar",
    "eod" // data base delimiter string
};

// LED patterns for state 3 (as specified in the requirements)

volatile char leds[9];
volatile char* leds_ptr = leds;

// string vars for state4
char str1[] = "An apple a day keeps the doctor away ";
char str2[] = "Strike while the iron's hot";

void main(void) {
  // Initialize system state and mode
  state = state0;        // Start in idle state on RESET
  lpm_mode = mode0;      // Start with low power mode 0
  
  char tmp[9] = {128, 224, 32, 126, 83, 44, 253, 113, 160};
  memcpy(leds, tmp, 9);
  leds_ptr = leds;
  // System configuration
  sysConfig();           // Configure GPIO, timers, DMA
  lcd_init();            // Initialize LCD
  
  // Clear LCD and show initial message
  lcd_clear();
  lcd_puts("System Ready");
  delay(2000);

  // Main operation loop
  while(1) {
    
    // Execute current state's functionality
      lcd_clear();
      clear_LEDs();
    switch(state) {
      case state0:
        // Idle state - waiting for button press
        //lcd_clear();
        lcd_puts("Press PB0-PB3");
        enterLPM(lpm_mode);
        break;
        
      case state1:
        // Idiom recorder state
        //clear_LEDs();    // Turn off all LEDs
        idiom_rec();     // Call idiom recorder function
        state = state0;  // Return to idle after completion
        break;
        
      case state2:
        // String mirror operation using DMA
        //clear_LEDs();
        strMirror();     // Call string mirror function
        // state is set to state0 within strMirror function
        break;
        
      case state3:
        // LED pattern cycling using DMA
        shift_leds();    // Call LED shift function
        // state is set to state0 within shift_leds function
        break;
        
      case state4:
        // realtime task state - Return to idle state
        merge();
        state = state0;
        break;

      default:
        // Invalid state - return to idle

        state = state0;
        break;
    }
  }
}
