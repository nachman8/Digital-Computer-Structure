#ifndef _api_H_
#define _api_H_

#include  "../header/hal_GPIO.h"     // private library - HAL layer


extern void printSWs2LEDs(void);
extern void printArr2SWs(char Arr[], int size, unsigned int rate);

extern void countLEDs(int);
// For state1
extern void updateLEDsState1(unsigned char* ledValue, unsigned char mode);
// For state2
extern void updateLEDsState2(unsigned char* ledValue);
// For state4
extern void PrintArr(int arr[] , int SIZE, char dir, int delayMS);

#endif







