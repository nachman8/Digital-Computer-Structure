#include "../header/bsp.h"

//-----------------------------------------------------------------------------  
//           GPIO Configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void) {
    WDTCTL = WDTPW + WDTHOLD;     // Stop WDT
    
    // LCD configuration (P1.4-P1.7 for data, P2.5-P2.7 for control)
    LCD_DATA_DIR |= 0xF0;          // P1.4-P1.7 set as outputs for LCD data
    LCD_DATA_SEL &= ~0xF0;         // GPIO functionality for P1.4-P1.7
    LCD_CTL_SEL &= ~0xE0;          // GPIO functionality for P2.5-P2.7
		
	// Input signal configuration (P2.4 for input capture)
	GenPortDir &= ~BIT4;           // P2.4 set as input
	GenPortSel |= BIT4;            // VERY IMPORTANT: Select Timer_A capture function
		
    // Buzzer configuration (P2.2 for PWM output)
    BuzzPortDir |= BIT2;           // P2.2 set as output
    BuzzPortDir &= ~BIT1;          // P2.1 set as input
    BuzzPortSel |= BIT2;           // Select Timer_A compare function
    BuzzPortSel &= ~BIT1;          // set P2.1 as GPIO
    BuzzPortOut &= ~BIT2;          // Initialize output low
    BuzzPortEdgeSel &= ~0x02;      // Set rising edge for interrupts
    BuzzPortIntEn |= 0x02;         // set interrupt enable
    BuzzPortIntPend &= ~0x02;      // clear interrupt flags

    // Pushbuttons configuration (P1.0-P1.2)
    PBsArrPortSel &= ~0x07;        // GPIO functionality for P1.0-P1.2
    PBsArrPortOut &= ~0x07;        // Initial value 0
    PBsArrPortDir &= ~0x07;        // Set as inputs
    PBsArrIntEdgeSel |= 0x03;      // Set falling edge for interrupts
    PBsArrIntEdgeSel &= ~0x04;      // Set rising edge for interrupts
    PBsArrIntEn |= 0x07;           // Enable interrupts for P1.0-P1.2
    PBsArrIntPend &= ~0x07;        // Clear pending interrupts
    //Pushbuttons configuration Realtime
    
	// In GPIOconfig(), modify the SW0 configuration:
	// Toggle switch configuration (P2.0)
	SWPortSel &= ~SW0;             // GPIO functionality for P2.0
	SWPortDir &= ~SW0;             // Set as input
	SWIntEdgeSel &= ~SW0;          // Low-to-high edge detection
	SWIntEn |= SW0;                // Enable interrupt
	SWIntPend &= ~SW0;             // Clear pending interrupt
    
    // Potentiometer ADC input configuration (P1.3)
    P1DIR &= ~POT_BIT;             // P1.3 as input
    P1SEL |= POT_BIT;              // Select ADC function
    
    // Enable global interrupts
    __bis_SR_register(GIE);
}

//-----------------------------------------------------------------------------
//           Timer B Configuration for Realtime (State 4)
//-----------------------------------------------------------------------------
/*void TimerB_Config(void) {
    TA0CTL = MC_0;                 // Stop timer

    // Configure for 1 second interval using SMCLK
    // For 1MHz SMCLK, divide by 8 → 125kHz, count to 125000 for 1 second
    // For 32.768kHz ACLK, use full count and divide appropriately
    TBCTL = TASSEL_2 + ID_3 + MC_0 + TACLR;  // SMCLK, divide by 8, stopped, clear
    TBCCR0 = 62500;               // 125000/2 for up/down mode = 1 second
    TBCCTL0 = CCIE;               // Enable interrupt on compare
}
*/

//-----------------------------------------------------------------------------
//           Timer A0 Configuration for Stopwatch (State 2)
//-----------------------------------------------------------------------------
void TimerA0_Config(void) {
    TA0CTL = MC_0;                 // Stop timer
    
    // Configure for 1 second interval using SMCLK
    // For 1MHz SMCLK, divide by 8 → 125kHz, count to 125000 for 1 second
    // For 32.768kHz ACLK, use full count and divide appropriately
    TA0CTL = TASSEL_2 + ID_2 + MC_0 + TACLR;  // SMCLK, divide by 4, stopped, clear
    TA0CCR0 = 62500;               // 125000/2 for up/down mode = 1 second
    TA0CCTL0 = CCIE;               // Enable interrupt on compare
}

//-----------------------------------------------------------------------------
//           Timer A1 Configuration for Input Capture (State 1)
//-----------------------------------------------------------------------------
void TimerA1_CaptureConfig(void) {
    // Stop timer and clear before configuration
    TA1CTL = MC_0 + TACLR;
    
    // Configure Timer A1 for input capture
    TA1CTL = TASSEL_2 + MC_2;  // SMCLK, continuous mode
    
    // Configure CCR2 for input capture
    TA1CCTL2 = CAP + CM_1 + SCS + CCIS_0;  // Capture mode, rising edge
                                           // synchronize, CCIxA input select
}

//-----------------------------------------------------------------------------
//           Timer A1 Configuration for PWM Output (State 3)
//-----------------------------------------------------------------------------
void TimerA1_PWMConfig(void) {
    TA1CTL = MC_0;                 // Stop timer
    
    // Configure Timer A1 with SMCLK for PWM on P2.2
    TA1CTL = TASSEL_2 + MC_1 + TACLR;  // SMCLK, up mode, clear
    TA1CCTL1 = OUTMOD_7;           // Reset/Set mode for PWM
    
    // Default settings for 1kHz @ 50% duty cycle
    TA1CCR0 = 1000;                // Period for 1kHz with 1MHz SMCLK
    TA1CCR1 = 500;                 // 50% duty cycle
}

//-----------------------------------------------------------------------------
//           ADC10 Configuration (for Tone Generator)
//-----------------------------------------------------------------------------
void ADCconfig(void) {
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;  // Sample and hold time, ADC10 on, enable interrupt
    ADC10CTL1 = POT_CHANNEL + ADC10SSEL_3;       // Input channel A3, SMCLK as clock source
    ADC10AE0 |= POT_BIT;                         // Enable analog input on P1.3
    
    // Do not enable conversion yet - will be enabled in the tone generator function
}
