#include "../header/halGPIO.h"

// Global variables
unsigned int REdge1 = 0, REdge2 = 0;  // For frequency measurement
unsigned int seconds = 0;             // For stopwatch
// stopwatch_running is defined in main.c

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void) { 
    GPIOconfig();
    TimerA0_Config();
    TimerA1_CaptureConfig();
    TimerA1_PWMConfig();
    ADCconfig();
}

//--------------------------------------------------------------------
//             Delay Functions
//--------------------------------------------------------------------
void delay(unsigned int t) {
    volatile unsigned int i;
    for(i = t; i > 0; i--);
}

void DelayMs(unsigned int ms) {
    unsigned int i;
    for(i = ms; i > 0; i--)
        DelayUs(1000);
}

void DelayUs(unsigned int us) {
    unsigned char i;
    // Each NOP takes about 1μs at 1MHz clock
    for(i = us; i > 0; i--)
        __no_operation();
}

//--------------------------------------------------------------------
//             Low Power Mode Functions
//--------------------------------------------------------------------
void enterLPM(unsigned char LPM_level) {
    switch(LPM_level) {
        case 0:
            __bis_SR_register(LPM0_bits); // Enter LPM0
            break;
        case 1:
            __bis_SR_register(LPM1_bits); // Enter LPM1
            break;
        case 2:
            __bis_SR_register(LPM2_bits); // Enter LPM2
            break;
        case 3:
            __bis_SR_register(LPM3_bits); // Enter LPM3
            break;
        case 4:
            __bis_SR_register(LPM4_bits); // Enter LPM4
            break;
    }
}

void enable_interrupts() {
    __bis_SR_register(GIE);
}

void disable_interrupts() {
    __bic_SR_register(GIE);
}

//--------------------------------------------------------------------
//             LCD Functions
//--------------------------------------------------------------------
void lcd_init() {
    char init_value;

    if (LCD_MODE == FOURBIT_MODE)
        init_value = 0x3 << LCD_DATA_OFFSET;
    else
        init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);  // Wait for LCD to power up
    
    // Send initialization sequence for 4-bit mode
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE) {
        LCD_WAIT;
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET;  // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28);  // Function Set: 4-bit, 2 lines, 5x8 dots
    } else {
        lcd_cmd(0x3C);  // 8-bit, 2 lines, 5x10 dots
    }

    lcd_cmd(0x0C);  // Display On, Cursor Off
    lcd_cmd(0x01);  // Clear Display
    lcd_cmd(0x06);  // Entry Mode Set: Increment cursor position, no display shift
    lcd_cmd(0x80);  // Set DDRAM address to 0
}

void lcd_cmd(unsigned char c) {
    LCD_WAIT;  // Wait for LCD to be ready

    if (LCD_MODE == FOURBIT_MODE) {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;               // Clear data bits
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;  // Send high nibble
        lcd_strobe();
        
        LCD_DATA_WRITE &= ~OUTPUT_DATA;               // Clear data bits
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;  // Send low nibble
        lcd_strobe();
    } else {
        LCD_DATA_WRITE = c;  // Send full byte
        lcd_strobe();
    }
}

void lcd_data(unsigned char c) {
    LCD_WAIT;  // Wait for LCD to be ready

    LCD_RS(1);  // Set RS for data
    
    if (LCD_MODE == FOURBIT_MODE) {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;               // Clear data bits
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;  // Send high nibble
        lcd_strobe();
        
        LCD_DATA_WRITE &= ~OUTPUT_DATA;               // Clear data bits
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;  // Send low nibble
        lcd_strobe();
    } else {
        LCD_DATA_WRITE = c;  // Send full byte
        lcd_strobe();
    }
    
    LCD_RS(0);  // Reset RS
}

void lcd_puts(const char* s) {
    while(*s)
        lcd_data(*s++);
}

void lcd_strobe() {
    LCD_EN(1);
    __no_operation();  // Brief delay
    LCD_EN(0);
}

//******************************************************************
//    Write frequency template to LCD
//******************************************************************

void write_stopwatch_LCD(unsigned int sec) {
    char time_str[6];
    unsigned int minutes = sec / 60;
    unsigned int seconds = sec % 60;
    
    // Format as MM:SS
    time_str[0] = (minutes / 10) + '0';
    time_str[1] = (minutes % 10) + '0';
    time_str[2] = ':';
    time_str[3] = (seconds / 10) + '0';
    time_str[4] = (seconds % 10) + '0';
    time_str[5] = '\0';
    
    // Display on LCD
    lcd_clear();
    lcd_home();
    lcd_puts(time_str);
}

void clear_LCD_line(unsigned char line) {
    unsigned char i;
    
    if (line == 0) {
        lcd_home();
    } else {
        lcd_new_line;
    }
    
    // Write 16 spaces to clear the line
    for (i = 0; i < 16; i++) {
        lcd_data(' ');
    }
    
    // Return cursor to beginning of line
    if (line == 0) {
        lcd_home();
    } else {
        lcd_new_line;
    }
}

//--------------------------------------------------------------------
//             Interrupt Service Routines
//--------------------------------------------------------------------

// Timer A0 ISR - For stopwatch
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TimerA0_ISR(void) {
    if (stopwatch_running && state == state2) {
        seconds++;
        write_stopwatch_LCD(seconds);
    }
    if (state == state4) {
        if (two_sec_counter % 2 == 0) {
            switch (two_sec_counter/2) {

            case 1:
                lcd_home();
                lcd_puts((char*)str_arr[0]);
                break;
            case 2:
                lcd_home();
                lcd_new_line;
                lcd_puts((char*)str_arr[1]);
                break;
            case 3:
                lcd_home();
                lcd_puts((char*)str_arr[2]);
                break;
            case 4:
                lcd_home();
                lcd_new_line;
                lcd_puts((char*)str_arr[3]);
                break;
            default:
                break;
            }
        }
        two_sec_counter = (two_sec_counter+1) % 8 ;
    }
}

// Timer A1 ISR - For input capture and PWM
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TimerA1_ISR(void) {
    switch(__even_in_range(TA1IV, 10)) {
        case 4:  // CCR2 CCIFG - Input Capture
            if (TA1CCTL2 & CCI) {  // Check if rising edge detected
                if (REdge1 == 0) {
                    REdge1 = TA1CCR2;
                } else {
                    REdge2 = TA1CCR2;
                    TA1CCTL2 &= ~CCIE;  // Disable interrupt after 2nd capture
                    __bic_SR_register_on_exit(LPM0_bits);  // Exit LPM0
                }
            }
            break;
    }
}

// ADC10 ISR - For potentiometer readings
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    // Exit LPM to process ADC result
    __bic_SR_register_on_exit(CPUOFF);
}

// Port 1 ISR - For pushbuttons PB0-PB2
#pragma vector=PORT1_VECTOR
__interrupt void PB_handler(void) {
    // Debounce delay
    delay(debounceVal);
    
    // Handle state transitions based on which button was pressed
    if (PBsArrIntPend & PB0) {
        state = state1;          // Frequency measurement
        PBsArrIntPend &= ~PB0;   // Clear interrupt flag
    } 
    else if (PBsArrIntPend & PB1) {
        state = state2;          // Stopwatch
        seconds = 0;             // Reset seconds counter
        PBsArrIntPend &= ~PB1;   // Clear interrupt flag
    } 
    else if (PBsArrIntPend & PB2) {
        state = state3;          // Tone generator
        PBsArrIntPend &= ~PB2;   // Clear interrupt flag
    }
    
    // Exit LPM to handle state change
    switch(lpm_mode) {
        case mode0: LPM0_EXIT; break;
        case mode1: LPM1_EXIT; break;
        case mode2: LPM2_EXIT; break;
        case mode3: LPM3_EXIT; break;
        case mode4: LPM4_EXIT; break;
    }
}
// Port 2 ISR - For toggle switch SW0
#pragma vector=PORT2_VECTOR
__interrupt void SW_handler(void) {
    // Debounce
    delay(debounceVal);

    // Check if switch interrupt is still pending
    if (SWIntPend & SW0) {
        if (state == state2) {  // Only act in stopwatch mode
            // Set stopwatch running state based on actual switch position
            stopwatch_running = (SWPort & SW0) ? 1 : 0;
        }
        SWIntPend &= ~SW0;  // Clear interrupt flag
    }
    // Check if PB2  is still pressed
    if (P2IFG & PB3){
        state = state4;
        P2IFG &= ~PB3;
    }

    // Exit LPM
    LPM0_EXIT;
}

    
