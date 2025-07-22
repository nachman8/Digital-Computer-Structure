#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;

char modeState3 = 1;    // 0: 4kHz/75%, 1: 2kHz/25%
char dir = 0;

void main(void){
  
	state = state0;  // start in idle state on RESET
	lpm_mode = mode0;     // start in idle state on RESET
	sysConfig();
	unsigned char val_state1 = 0; // Initial value for state1 LED tracking
	unsigned char mode1 = 0;        // 0: increment, 1: decrement
	unsigned char val_state2 = 0; // Initial value for state2 LED tracking
	unsigned char mode2 = 0;      // 0: right to left, 1: left to right


	int ID[] = {'3','2','1','7','3','0','5','5'};
	int sizeID = 8;
	int delayTime = 250;

  
	while(1){
	switch(state){
	  case state0:
		clrLEDs();
		enterLPM(lpm_mode);
		break;
		 
	  case state1:
		disable_interrupts();
		updateLEDsState1(&val_state1, mode1);
		mode1 ^= 1; // toggle mode
		state = state0;
		enable_interrupts();
		break;
		 
	  case state2:
		disable_interrupts();
		// Call the function for LED shifting
		updateLEDsState2(&val_state2);
		state = state0;
		enable_interrupts();
		break;
		
      case state3:
          {
            unsigned int Nperiod, Nduty, Ncycles; // Select parameters based on current mode
            enable_interrupts();
            if(modeState3 == 0) {
              Nperiod = period4k;
              Nduty = duty4k;
            }
            else {
              Nperiod = period2k;
              Nduty = duty2k;
            }
            Ncycles = 0xFFFF;  // Infinite cycles
            // Start PWM - this will run indefinitely until interrupted
            pwm(Nperiod, Nduty, Ncycles);
            // If we get here, PWM was interrupted
          }
          break;

      case state4:

          disable_interrupts();
          clrLEDs();
          PrintArr(ID, sizeID, dir , delayTime);

          state = state0;
          enable_interrupts();
          break;
	    }
	  } 
	} 






