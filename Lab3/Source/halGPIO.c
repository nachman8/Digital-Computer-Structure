#include "../header/halGPIO.h"     // Hardware abstraction layer
#include <stdio.h>
#include <string.h>

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void) {
    GPIOconfig();     // Initialize GPIO ports
    TIMER_config();   // Initialize timers
    DMA_config();     // Initialize DMA controller
}

//--------------------------------------------------------------------
//            Polling-based Delay function
//--------------------------------------------------------------------
void delay(unsigned int t) {  // t[msec]
    volatile unsigned int i;
    
    for(i=t; i>0; i--);
}

//--------------------------------------------------------------------
//            Enter Low-Power Mode
//--------------------------------------------------------------------
void enterLPM(unsigned char LPM_level) {
    switch (LPM_level) {
        case 0:
            _BIS_SR(LPM0_bits + GIE);     // Enter LPM0 with interrupts enabled
            break;
        case 1:
            _BIS_SR(LPM1_bits + GIE);     // Enter LPM1 with interrupts enabled
            break;
        case 2:
            _BIS_SR(LPM2_bits + GIE);     // Enter LPM2 with interrupts enabled
            break;
        case 3:
            _BIS_SR(LPM3_bits + GIE);     // Enter LPM3 with interrupts enabled
            break;
        case 4:
            _BIS_SR(LPM4_bits + GIE);     // Enter LPM4 with interrupts enabled
            break;
        default:
            _BIS_SR(LPM0_bits + GIE);     // Default to LPM0 with interrupts enabled
    }
}

//--------------------------------------------------------------------
//            LCD Command Function
//--------------------------------------------------------------------
void lcd_cmd(unsigned char c) {
    LCD_WAIT;                             // Wait until LCD is ready

    if (LCD_MODE == FOURBIT_MODE) {
        // 4-bit mode requires two transfers (upper nibble then lower nibble)
        LCD_DATA_WRITE &= ~OUTPUT_DATA;   // Clear data bits
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;  // Send upper nibble
        lcd_strobe();                     // Strobe to latch data

        LCD_DATA_WRITE &= ~OUTPUT_DATA;   // Clear data bits
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;         // Send lower nibble
        lcd_strobe();                     // Strobe to latch data
    } else {
        // 8-bit mode sends all bits at once
        LCD_DATA_WRITE = c;               // Send full byte
        lcd_strobe();                     // Strobe to latch data
    }
}

//--------------------------------------------------------------------
//            LCD Data Function
//--------------------------------------------------------------------
void lcd_data(unsigned char c) {
    LCD_WAIT;                             // Wait until LCD is ready

    LCD_DATA_WRITE &= ~OUTPUT_DATA;       // Clear data bits
    LCD_RS(1);                            // Set RS high for data

    if (LCD_MODE == FOURBIT_MODE) {
        // Send upper nibble
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();

        // Clear and prepare for lower nibble
        LCD_DATA_WRITE &= ~OUTPUT_DATA;

        // Send lower nibble
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
    } else {
        // 8-bit mode sends all bits at once
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }

    LCD_RS(0);                            // Reset RS to command mode
}

//--------------------------------------------------------------------
//            LCD String Display Function
//--------------------------------------------------------------------
void lcd_puts(const char * s) {
    int char_count = 0;

    while(*s) {                           // Process until null terminator
        lcd_data(*s++);                   // Display character and increment pointer
        char_count++;

        // Handle line wrapping
        if(char_count == 16) {            // End of first line reached
            lcd_new_line;                 // Move to second line
        }

/*        // Handle pagination for long text
        if(char_count == 32) {            // End of screen reached
            // Enable keypad to allow user to continue
            en_keypad_interrupts();
            enterLPM(mode0);              // Wait for key press
            disable_keypad_interrupts();

            // Check if # key was pressed for clear screen
            if(Key == 14) {
                lcd_clear();              // Clear display
                char_count = 0;           // Reset counter
            }
        }
*/    }
}

//--------------------------------------------------------------------
//            LCD Initialization
//--------------------------------------------------------------------
void lcd_init() {
    char init_value;

    // Determine initialization value based on LCD mode
    if (LCD_MODE == FOURBIT_MODE)
        init_value = 0x3 << LCD_DATA_OFFSET;
    else
        init_value = 0x3F;

    // Configure control pins as outputs
    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;

    // Initialize control pins to low
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    // LCD startup sequence (standard HD44780 initialization)
    DelayMs(15);                          // Wait >15ms after power on

    // First initialization command
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    // Second initialization command
    DelayMs(5);                           // Wait >4.1ms
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    // Third initialization command
    DelayUs(200);                         // Wait >100us
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    // Set up for 4-bit or 8-bit mode
    if (LCD_MODE == FOURBIT_MODE) {
        LCD_WAIT;
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET;  // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28);                    // Function Set: 4-bit, 2 lines, 5x8 dots
    } else {
        lcd_cmd(0x3C);                    // Function Set: 8-bit, 2 lines, 5x10 dots
    }

    // Finish initialization
    lcd_cmd(0x0F);                        // Display On, Cursor On, Blink On
    lcd_cmd(0x01);                        // Clear Display
    lcd_cmd(0x06);                        // Entry Mode: Increment, No Shift
    lcd_cmd(0x80);                        // Set DDRAM Address to 0
    lcd_cmd(0x0C);                        // Display On, Cursor Off, Blink Off
}

//--------------------------------------------------------------------
//            Delay functions
//--------------------------------------------------------------------
void DelayUs(unsigned int count) {
    unsigned char i;
    for(i=count; i>0; i--)
        asm("nop");                       // Each nop takes approximately 1us
}

void DelayMs(unsigned int count) {
    unsigned char i;
    for(i=count; i>0; i--)
        DelayUs(1000);                    // 1ms = 1000us
}

//--------------------------------------------------------------------
//            LED Control
//--------------------------------------------------------------------
void clear_LEDs() {
    LEDsArrPort = 0x00;                   // Turn off all LEDs
}

//--------------------------------------------------------------------
//            LCD Strobe Function
//--------------------------------------------------------------------
void lcd_strobe() {
    LCD_EN(1);                            // Set enable high
    asm("NOP");                           // Small delay
    LCD_EN(0);                            // Set enable low to latch data
}

//--------------------------------------------------------------------
//            Interrupt Control
//--------------------------------------------------------------------
void enable_interrupts() {
    _BIS_SR(GIE);                         // Set Global Interrupt Enable bit
}

void disable_interrupts() {
    _BIC_SR(GIE);                         // Clear Global Interrupt Enable bit
}

//--------------------------------------------------------------------
//            Keypad Interrupt Control
//--------------------------------------------------------------------
void en_keypad_interrupts() {
    KeypadIRQIntPend &= ~BIT1;            // Clear pending interrupt flag
    KeypadIRQIntEn |= BIT1;               // Enable keypad interrupt
}

void disable_keypad_interrupts() {
    KeypadIRQIntEn &= ~BIT1;              // Disable keypad interrupt
}

//--------------------------------------------------------------------
//            Timer Control
//--------------------------------------------------------------------
void startTimerA() {
    //Timer0_CCR0 = 32768;                  // ~1 second timeout for multi-press
    Timer0_CTL = TASSEL_1 + MC_1 + TACLR; // ACLK, up mode, clear timer
    Timer0_CTL |= TAIE;                   // Enable Timer_A interrupt
}

void startTimerB() {
    //Timer1_CCR0 = 62500;                  // 500ms at 1MHz
    Timer1_CTL = TBSSEL_2 + MC_1 + ID_3 + TBCLR; // SMCLK, up mode, /8, clear timer
    Timer1_CTL |= TBIE;                   // Enable Timer_B interrupt
}

void finishTimerA() {
    Timer0_CTL &= ~(TAIE + MC_3);         // Disable Timer_A interrupt and stop
    Timer0_CTL |= TACLR;                  // Clear timer
}

void finishTimerB() {
    Timer1_CTL &= ~(TBIE + MC_3);         // Disable Timer_A interrupt and stop
    Timer1_CTL |= TACLR;                  // Clear timer
}


//--------------------------------------------------------------------
//            DMA Control
//--------------------------------------------------------------------
void startDMA(){
    if(state==state2) {      // state2 - Mirror
        DMA0CTL = DMADT_1 + DMASBDB + DMASRCINCR_2 + DMADSTINCR_3; // block, byte to byte, src dec, dst inc
    }
    else if(state==state3) { // state 3 - LEDS
        DMA0CTL = DMADT_1 + DMASBDB + DMADSTINCR_0 + DMASRCINCR_0; // block, byte to byte, no inc
    }
    else if(state==state4) { // state 4 - String Merge
        // Configure DMA0 for first string transfer
        DMA0CTL = DMADT_1 + DMASBDB + DMASRCINCR_3 + DMADSTINCR_3 + DMAIE; // block, byte-to-byte, src inc, dst inc, interrupt enabled

        // Configure DMA1 for second string transfer
        DMA1CTL = DMADT_1 + DMASBDB + DMASRCINCR_3 + DMADSTINCR_3 + DMAIE; // block, byte-to-byte, src inc, dst inc, interrupt enabled
    }
}

void stopDMA() {
    DMA0CTL &= ~DMAEN;                    // Disable DMA channel
}

void triggerDMA() {
    DMA0CTL |= DMAEN + DMAREQ;            // Enable DMA and trigger transfer
}

void triggerDMA0(void) {
    DMA0CTL |= DMAEN + DMAREQ;            // Enable DMA0 and trigger transfer
}

void triggerDMA1(void) {
    DMA1CTL |= DMAEN + DMAREQ;            // Enable DMA1 and trigger transfer
}
void set_dma_src_addr(__SFR_FARPTR addr) {
    DMA0SA = addr;
}

void set_dma_dst_addr(__SFR_FARPTR addr) {
    DMA0DA = addr;
}
//--------------------------------------------------------------------
//            Interrupt Service Routines
//--------------------------------------------------------------------

// PORT1 Interrupt Service Routine (Pushbuttons)
#pragma vector=PORT1_VECTOR
__interrupt void PBs_handler(void) {
    delay(debounceVal);                   // Debounce delay
    //yardenl - manually switched PB0 and PB3 because the lab's kit is broken
    // Check which button was pressed and set state accordingly
    if(PBsArrIntPend & PB0) {
        state = state4;                   // Idiom recorder state
        PBsArrIntPend &= ~PB0;            // Clear interrupt flag
    }
    else if(PBsArrIntPend & PB1) {
        state = state2;                   // Word operations state
        PBsArrIntPend &= ~PB1;            // Clear interrupt flag
    }
    else if(PBsArrIntPend & PB2) {
        state = state3;                   // LED shifting state
        PBsArrIntPend &= ~PB2;            // Clear interrupt flag
    }
    else if(PBsArrIntPend & PB3) {
        state = state1;                   // Return to idle state
        PBsArrIntPend &= ~PB3;            // Clear interrupt flag
    }

    // Exit from current low-power mode
    switch(lpm_mode) {
        case mode0: LPM0_EXIT; break;     // Exit from LPM0
        case mode1: LPM1_EXIT; break;     // Exit from LPM1
        case mode2: LPM2_EXIT; break;     // Exit from LPM2
        case mode3: LPM3_EXIT; break;     // Exit from LPM3
        case mode4: LPM4_EXIT; break;     // Exit from LPM4
    }
}

// PORT2 Interrupt Service Routine (Keypad)
#pragma vector=PORT2_VECTOR
__interrupt void keypad_handler(void) {
    delay(debounceVal);                   // Debounce delay

    // Process keypad input
    if(KeypadIRQIntPend & BIT1) {
        Key = 16;                         // Default to invalid key (16 = no key)

        // Scan first row (Row 1)
        KeypadPortOUT = 0x0E;             // Set Row 1 low, others high
        DelayUs(10);                      // Small delay for settling
        if((KeypadPortIN & 0x10) == 0)      Key = 15;  // Column 1
        else if((KeypadPortIN & 0x20) == 0) Key = 14;  // Column 2
        else if((KeypadPortIN & 0x40) == 0) Key = 13;  // Column 3
        else if((KeypadPortIN & 0x80) == 0) Key = 12;  // Column 4

        // Scan second row (Row 2)
        KeypadPortOUT = 0x0D;             // Set Row 2 low, others high
        DelayUs(10);
        if((KeypadPortIN & 0x10) == 0)      Key = 11;  // Column 1
        else if((KeypadPortIN & 0x20) == 0) Key = 10;  // Column 2
        else if((KeypadPortIN & 0x40) == 0) Key = 9;   // Column 3
        else if((KeypadPortIN & 0x80) == 0) Key = 8;   // Column 4

        // Scan third row (Row 3)
        KeypadPortOUT = 0x0B;             // Set Row 3 low, others high
        DelayUs(10);
        if((KeypadPortIN & 0x10) == 0)      Key = 7;   // Column 1
        else if((KeypadPortIN & 0x20) == 0) Key = 6;   // Column 2
        else if((KeypadPortIN & 0x40) == 0) Key = 5;   // Column 3
        else if((KeypadPortIN & 0x80) == 0) Key = 4;   // Column 4

        // Scan fourth row (Row 4)
        KeypadPortOUT = 0x07;             // Set Row 4 low, others high
        DelayUs(10);
        if((KeypadPortIN & 0x10) == 0)      Key = 3;   // Column 1
        else if((KeypadPortIN & 0x20) == 0) Key = 2;   // Column 2
        else if((KeypadPortIN & 0x40) == 0) Key = 1;   // Column 3
        else if((KeypadPortIN & 0x80) == 0) Key = 0;   // Column 4

        // Reset keypad
        KeypadPortOUT = 0x00;             // Reset all rows
        DelayUs(10);
        KeypadIRQIntPend &= ~BIT1;        // Clear interrupt flag
    }

    // Exit from current low-power mode
    switch(lpm_mode) {
        case mode0: LPM0_EXIT; break;     // Exit from LPM0
        case mode1: LPM1_EXIT; break;     // Exit from LPM1
        case mode2: LPM2_EXIT; break;     // Exit from LPM2
        case mode3: LPM3_EXIT; break;     // Exit from LPM3
        case mode4: LPM4_EXIT; break;     // Exit from LPM4
    }
}

// Timer A Interrupt Service Routine
#pragma vector = TIMERA1_VECTOR
__interrupt void TimerA_ISR(void) {
    if(Timer0_CTL & TAIFG) {              // Check if interrupt is from Timer A
        Timer0_CTL &= ~TAIFG;             // Clear interrupt flag
        LPM0_EXIT;                        // Exit from LPM0
    }
}

// Timer B Interrupt Service Routine
#pragma vector = TIMERB1_VECTOR
__interrupt void TimerB_ISR(void) {
    if(Timer1_CTL & TBIFG) {              // Check if interrupt is from Timer B
        Timer1_CTL &= ~TBIFG;             // Clear interrupt flag
        LPM0_EXIT;                        // Exit from LPM0
    }
}


#pragma vector=DMA_VECTOR
__interrupt void DMA_ISR(void) {
    // Check which DMA channel triggered the interrupt
    if (DMA0CTL & DMAIFG) {
        DMA0CTL &= ~DMAIFG;  // Clear DMA0 interrupt flag

        // Exit low power mode if it was waiting
        switch (lpm_mode) {
            case mode0: LPM0_EXIT; break;
            case mode1: LPM1_EXIT; break;
            case mode2: LPM2_EXIT; break;
            case mode3: LPM3_EXIT; break;
            case mode4: LPM4_EXIT; break;
        }
    }
    else if (DMA1CTL & DMAIFG) {
        DMA1CTL &= ~DMAIFG;  // Clear DMA1 interrupt flag

        // Exit low power mode if it was waiting
        switch (lpm_mode) {
            case mode0: LPM0_EXIT; break;
            case mode1: LPM1_EXIT; break;
            case mode2: LPM2_EXIT; break;
            case mode3: LPM3_EXIT; break;
            case mode4: LPM4_EXIT; break;
        }
    }
}
