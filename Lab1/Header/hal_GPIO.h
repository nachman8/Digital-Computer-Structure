#ifndef _halGPIO_H_
#define _halGPIO_H_


#include "../header/bsp_msp430x2xx.h"
//#include "../header/bsp_msp430x4xx.h"

#include  "../header/app.h"    		// private library - APP layer


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern char modeState3;
extern char dir;

extern void sysConfig(void);
extern void print2LEDs(unsigned char);
extern void clrLEDs(void);
extern void toggleLEDs(char);
extern void setLEDs(char);
extern unsigned char readSWs(void);
extern void delay(unsigned int);
extern void delay_ms(unsigned int t);
extern void enterLPM(unsigned char);
extern void incLEDs(char);
extern void enable_interrupts();
extern void disable_interrupts();
// For state3 - PWM function
extern void pwm(unsigned int Nperiod, unsigned int Nduty, unsigned int Ncycles);


extern __interrupt void PBs_handler(void);

#endif







