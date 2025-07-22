#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern unsigned int num;
extern int k;
extern int print_negev_flag;

extern void blinkRGB(void);
extern void count(void);
extern void buzzer(void);
extern void set_X(void);
extern void measPOT(void);
extern void print_string();
extern void test_qformat();
extern void display_ascii();

#endif
