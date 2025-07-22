#include  "../header/api.h"         // private library - API layer
#include  "../header/app.h"         // private library - APP layer
#include "../header/iqmath_msp430.h"

enum FSMstate state;
enum SYSmode lpm_mode;

unsigned int num = 0;
unsigned int X = 0xFFFF;
unsigned int X_flag = 0;
char timerInput[6];
char sentence[]={"I love my Negev\n"};
int k = 16;
int print_negev_flag=0;

char ascii_char = 0;
int ascii_display_flag = 0;

void main(void){

  

  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_init();
  test_qformat();

  // Test LCD
  lcd_puts("LCD Ready");
  DelayMs(300);  // Show for 3 seconds
  lcd_clear();    // Clear the test message
  DelayMs(50);

  enable_UARTRX_interrupts();

  while(1){
    // DO NOT PUT lcd_clear() HERE! This was the main problem!
    switch(state){
      case state0:                                      // Sleep mode
          enable_UARTRX_interrupts();          // Enable USCI_A0 RX interrupt
          enterLPM(lpm_mode);                   // enter low power mode
          break;

      case state1:                                      // Count LCD
          enable_UARTRX_interrupts();           // Enable USCI_A0 RX interrupt
          count();                                 // count up to LCD
          break;

      case state2:                                      // Buzzer
          enable_UARTRX_interrupts();
          buzzer();                                   // shift buzzer frequency
          break;

      case state3:                                      // Set X
            set_X();
            state = state0;
            break;

      case state4:                              // Measure POT
          enable_UARTRX_interrupts();
          measPOT();
          break;

      case state5:                          // Send message on PB1 press
          enable_UARTRX_interrupts();
          print_string();
          state = state0;
          k = 16;
          break;

      case state6:                          // Clear LCD screen
          lcd_clear();                      // Only clear here when requested
          DelayMs(100);                     // Give time to process
          num = 0;                          // Reset counter
          state = state0;                   // Return to sleep
          break;

      case state7:                          // NEW: Display ASCII code
            //enable_UARTRX_interrupts();
            display_ascii();
            break;

      case state8:                          // Show menu (was state7)
            enable_UARTRX_interrupts();
            state = state0;
            break;

       case state9:                          // Sleep (was state8)
            enable_UARTRX_interrupts();
            enterLPM(lpm_mode);
            break;
      }
  }
}
