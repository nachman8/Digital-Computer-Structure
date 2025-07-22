#ifndef _bsp_H_
#define _bsp_H_

#include <msp430g2553.h>          // For MSP430x2xx series
// #include <msp430xG46x.h>       // For MSP430x4xx series (uncomment when using x4xx)

// Common constants
#define debounceVal            250
#define INPUT_SIGNAL_MIN_FREQ  50    // Hz
#define INPUT_SIGNAL_MAX_FREQ  20000 // Hz
#define TONE_MIN_FREQ          1000  // Hz
#define TONE_MAX_FREQ          2500  // Hz

// LCD configuration (4-bit mode)
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL

// Input signal generator pin (for frequency measurement)
#define GenPort            P2IN
#define GenPortSel         P2SEL
#define GenPortDir         P2DIR

// Input signal PB3 pin (for Real time task)
#define PB3Port            P2IN
#define PB3PortSel         P2SEL
#define PB3PortDir         P2DIR

// PWM Buzzer output pin configuration
#define BuzzPortSel        P2SEL
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT
#define BuzzPortIntEn      P2IE
#define BuzzPortEdgeSel    P2IES
#define BuzzPortIntPend    P2IFG




// Pushbuttons configuration
#define PBsArrPort         P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn        P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PBsArrPortOut      P1OUT
#define PB0                0x01   // P1.0
#define PB1                0x02   // P1.1
#define PB2                0x04   // P1.2
#define PB3                0x02   // P2.1


// Toggle switch configuration
#define SWPort             P2IN
#define SWIntPend          P2IFG
#define SWIntEn            P2IE
#define SWIntEdgeSel       P2IES
#define SWPortSel          P2SEL
#define SWPortDir          P2DIR
#define SW0                0x01   // P2.0

// Potentiometer ADC input configuration
#define POT_CHANNEL        INCH_3  // ADC input channel A3 (P1.3)
#define POT_BIT            BIT3    // P1.3

// Function prototypes
extern void GPIOconfig(void);
extern void TimerA0_Config(void);      // For Stopwatch (State 2)
extern void TimerA1_CaptureConfig(void); // For Frequency Measurement (State 1)
extern void TimerA1_PWMConfig(void);   // For Tone Generator (State 3)
extern void ADCconfig(void);

#endif
