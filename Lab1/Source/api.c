#include  "../header/api.h"    		// private library - API layer
#include  "../header/hal_GPIO.h"     // private library - HAL layer

//-------------------------------------------------------------
//            Print SWs value onto LEDs
//-------------------------------------------------------------
void printSWs2LEDs(void){
	unsigned char ch;
	
	ch = readSWs();
	print2LEDs(ch);
}
//--------------------------------------------------------------------
//            Print array to LEDs array with rate of LEDs_SHOW_RATE
//--------------------------------------------------------------------            
void printArr2SWs(char Arr[], int size, unsigned int rate){
	unsigned int i;
	
	for(i=0; i<size; i++){
		print2LEDs(Arr[i]);
		delay(rate);
	}
}
//---------------------------------------------------------------------
//            Function to count LEDs either up or down
//--------------------------------------------------------------------- 
void countLEDs(int direction) {
    volatile unsigned int count1 = 20;  // Initialize the counter to 20 iterations
    while (count1--) {                   // Count down from 20 to 0
        // Update LED count based on the direction provided
        incLEDs(direction);
        print2LEDs(LEDsArrPort);
        delay_ms(500);
    }
}
//---------------------------------------------------------------------
//            Update LEDs for State1 - continuous increment or decrement 
//---------------------------------------------------------------------
void updateLEDsState1(unsigned char* ledValue, unsigned char mode) {
    LEDsArrPort = *ledValue;
    print2LEDs(LEDsArrPort);
    TA0CTL = TASSEL_2 | MC_2 | TACLR;
    if (mode == 0) {
        countLEDs(1);
    } else {
        countLEDs(-1);
    }
    *ledValue = LEDsArrPort;
}
//---------------------------------------------------------------------
//            Update LEDs for State2 - shift LED from right to left
//---------------------------------------------------------------------
void updateLEDsState2(unsigned char* ledValue) {
    unsigned int count2;
    
    // If this is the first time or we need to reset
    if (*ledValue == 0 || *ledValue == 0xFF)
        *ledValue = 0x01;    // Start from rightmost LED
    
    // Shift LED for 14 iterations (7 positions each way)
    for(count2 = 0; count2 < 14; count2++) {
        // Update LEDs with current value
        print2LEDs(*ledValue);
        delay_ms(500);  // 0.5 sec delay
		// Shift left
		*ledValue = *ledValue << 1;
		// If we've gone past the last bit, restart from the first bit
		if (*ledValue == 0) *ledValue = 0x01;
    }
}

//---------------------------------------------------------------------
//            Print ID to LEDs for State4
//---------------------------------------------------------------------
void PrintArr(int arr[] , int SIZE, char dir ,int delayMS) {
    int i;
    for(i = 0; i < SIZE; i++){
        if (dir == 0)    print2LEDs(arr[i]-0x30);
        else             print2LEDs(arr[SIZE-i-1]-0x30);
        delay_ms(delayMS);

    }
}




 
  

