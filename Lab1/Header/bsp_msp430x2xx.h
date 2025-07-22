#ifndef _bsp_H_2_
#define _bsp_H_2_

#include  <msp430g2553.h>          // MSP430x2xx


#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms

// LEDs abstraction
#define LEDsArrPort        P1OUT
#define LEDsArrPortDir     P1DIR
#define LEDsArrPortSel     P1SEL

// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0x0F

// PushButtons abstraction
#define PBsArrPort	   P2IN 
#define PBsArrIntPend	   P2IFG 
#define PBsArrIntEn	   P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL 
#define PBsArrPortDir      P2DIR 
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08


// Define PWM parameters
// For MCLK = 1MHz, 1ms = 1000 cycles
// For 4kHz PWM: period = 250 cycles (1,000,000/4,000)
// For 2kHz PWM: period = 500 cycles (1,000,000/2,000)
#define period4k 22     // PWM period for 4kHz
#define duty4k   19     // 75% duty cycle for 4kHz (250*0.75=187.5)
#define period2k 52     // PWM period for 2kHz
#define duty2k   13     // 25% duty cycle for 2kHz (500*0.25=125)

// PWM output pin definition
#define PWM_PORT_DIR P2DIR
#define PWM_PORT_SEL P2SEL
#define PWM_PORT_OUT P2OUT
#define PWM_PIN      BIT7


extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);
extern void PWMconfig(void);

#endif



