// Board Support Package - Hardware Configuration

// Include appropriate BSP header based on processor
#ifdef __MSP430FG4619__
#include "../header/bsp_msp430x4xx.h"
#else
#include "../header/bsp_msp430x2xx.h"
#endif

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void) {
  
  // Stop watchdog timer to prevent timeout reset
  WDTCTL = WDTHOLD | WDTPW;
  
  //--------------------------------------------------------------------
  // LCD Configuration
  //--------------------------------------------------------------------
  LCD_DATA_WRITE &= ~0xFF;           // Clear data lines
  LCD_DATA_DIR |= 0xF0;              // P1.4-P1.7 as outputs
  LCD_DATA_SEL &= ~0xF0;             // Select GPIO function
  LCD_CTL_SEL  &= ~0xE0;             // Select GPIO function for control pins
  LCD_CTL_DIR  |= 0xE0;              // P2.5-P2.7 as outputs
  
  //--------------------------------------------------------------------
  // LED Configuration
  //--------------------------------------------------------------------
  LEDsArrPort = 0x00;                // Clear LED outputs
  LEDsArrPortDir |= 0xFF;            // Set LED port as output
  LEDsArrPortSel &= ~0xFF;           // Select GPIO function
  
  //--------------------------------------------------------------------
  // Keypad Configuration
  //--------------------------------------------------------------------
  KeypadPortSel &= ~0xFF;            // Select GPIO function
  KeypadPortDIR = 0x0F;              // P10.0-P10.3 as outputs (rows), P10.4-P10.7 as inputs (columns)
  KeypadPortOUT = 0x00;              // Clear row outputs
  
  //--------------------------------------------------------------------
  // Keypad IRQ Configuration
  //--------------------------------------------------------------------
  KeypadIRQPortSel &= ~BIT1;         // Select GPIO function for P2.1
  KeypadIRQPortDir &= ~BIT1;         // Set P2.1 as input
  KeypadIRQIntEdgeSel |= BIT1;       // Set for high-to-low transition (pull-up mode)
  KeypadIRQIntEn &= ~BIT1;           // Disable interrupts initially
  KeypadIRQIntPend &= ~0xFF;         // Clear pending interrupts
  
  //--------------------------------------------------------------------
  // Pushbutton Configuration
  //--------------------------------------------------------------------
  // Configure P1.0-P1.3 as inputs for pushbuttons
  PBsArrPortSel &= ~(PB0 + PB1 + PB2 + PB3);    // Select GPIO function
  PBsArrPortDir &= ~(PB0 + PB1 + PB2 + PB3);    // Set as inputs
  
  // Configure interrupt edge select
  PBsArrIntEdgeSel |= (PB0 + PB1);        // High-to-low for PB0-1 (pull-up)
  PBsArrIntEdgeSel &= ~(PB2 + PB3);                      // Low-to-high for PB2-3 (pull-down)
  
  // Enable interrupts and clear flags
  PBsArrIntEn |= (PB0 + PB1 + PB2 + PB3);       // Enable interrupts
  PBsArrIntPend &= ~(PB0 + PB1 + PB2 + PB3);    // Clear pending flags
  
  // Enable global interrupts
  _BIS_SR(GIE);
}

//------------------------------------------------------------------------------------- 
//             Timer configuration
//-------------------------------------------------------------------------------------
void TIMER_config(void) {
    // Configure Timer A (Timer0)
    Timer0_CCR0 = 0x8000;                           // count value = 32768
    Timer0_CCR2 = 0x7900;
    Timer0_CTL = TASSEL_1 + MC_1 + TACLR;           // ACLK, up mode, clear timer    Timer0_CTL &= ~TAIE;                                 // Disable Timer A interrupt

    // Configure Timer B (Timer1)
    Timer1_CCR0 = 0xF424;                           //  count value = 62500
    Timer1_CCR2 = 0xF300;
    Timer1_CTL = TBSSEL_2 + MC_1 + ID_3 + TBCLR;    // SMCLK, up mode, /8, clear timer
    Timer1_CTL &= ~TBIE;                            // Disable Timer B interrupt
}

//-------------------------------------------------------------------------------------
//            DMA configuration
//-------------------------------------------------------------------------------------
void DMA_config(void) {
    // Disable DMA interrupts initially
    DMA0CTL &= ~DMAIE;

    // Clear DMA control registers
    DMA0CTL = 0;
    DMA0_Src_Add = 0;
    DMA0_Dst_Add = 0;
    DMA0_Size = 0;

    // DMA channels are configured in specific state handlers
}
