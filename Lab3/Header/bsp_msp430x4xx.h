#ifndef _bsp_H_
#define _bsp_H_

#include <msp430xG46x.h>

// Define debounce value for button press
#define debounceVal      250

//--------------------------------------------------------------------
// LCD configuration
//--------------------------------------------------------------------
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL
#define LCD_CTL_DIR        P2DIR
#define LCD_CTL_OUT        P2OUT
#define LCD_EN_PIN         BIT5
#define LCD_RS_PIN         BIT6
#define LCD_RW_PIN         BIT7

//--------------------------------------------------------------------
// LED configuration
//--------------------------------------------------------------------
#define LEDsArrPort        P9OUT
#define LEDsArrPortDir     P9DIR
#define LEDsArrPortSel     P9SEL

//--------------------------------------------------------------------
// Timer configuration
//--------------------------------------------------------------------
#define Timer0_CCR2        TACCR2
#define Timer0_CCR0        TACCR0
#define Timer0_CTL         TACTL
#define Timer0_CCTL0       TACCTL0
#define Timer0_CCTL2       TACCTL2
#define Timer1_CCR0        TBCCR0
#define Timer1_CTL         TBCTL
#define Timer1_CCR2        TBCCR2
#define Timer1_CCTL2       TBCCTL2


//--------------------------------------------------------------------
// DMA configuration
//--------------------------------------------------------------------
#define DMA0_Src_Add       DMA0SA
#define DMA0_Dst_Add       DMA0DA
#define DMA0_Size          DMA0SZ

#define DMA1_Src_Add       DMA1SA
#define DMA1_Dst_Add       DMA1DA
#define DMA1_Size          DMA1SZ

//--------------------------------------------------------------------
// Pushbutton configuration
//--------------------------------------------------------------------
#define PBsArrPort         P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn        P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08

//--------------------------------------------------------------------
// Keypad configuration
//--------------------------------------------------------------------
#define KeypadPortSel      P10SEL
#define KeypadPortDIR      P10DIR
#define KeypadPortOUT      P10OUT
#define KeypadPortIN       P10IN
#define KeypadIRQPort      P2IN
#define KeypadIRQIntPend   P2IFG
#define KeypadIRQIntEn     P2IE
#define KeypadIRQIntEdgeSel P2IES
#define KeypadIRQPortSel   P2SEL
#define KeypadIRQPortDir   P2DIR
#define KeypadIRQPortOut   P2OUT

//--------------------------------------------------------------------
// Switch configuration
//--------------------------------------------------------------------
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0x01
#define SW0                0x01
#define SW1                0x02
#define SW2                0x04
#define SW3                0x08



//--------------------------------------------------------------------
// Function prototypes
//--------------------------------------------------------------------
extern void GPIOconfig(void);
extern void TIMER_config(void);
extern void DMA_config(void);

#endif
