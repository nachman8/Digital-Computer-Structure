#ifndef _halGPIO_H_
#define _halGPIO_H_

// Include appropriate BSP header based on processor
#ifdef __MSP430FG4619__
#include "../header/bsp_msp430x4xx.h"
#else
#include "../header/bsp_msp430x2xx.h"
#endif

#include "../header/app.h"    // private library - APP layer

// External global variables
extern enum FSMstate state;   // Current system state
extern enum SYSmode lpm_mode; // Current low power mode
extern int Key;               // Current key pressed

// System configuration functions
extern void sysConfig(void);                 // Configure system peripherals
extern void delay(unsigned int);             // Polling-based delay
extern void enterLPM(unsigned char);         // Enter specified low power mode
extern void enable_interrupts(void);         // Enable global interrupts
extern void disable_interrupts(void);        // Disable global interrupts

// Timer functions
extern void startTimerA(void);               // Start Timer A
extern void startTimerB(void);               // Start Timer B
extern void finishTimerA(void);              // Stop Timer A
extern void finishTimerB(void);              // Stop Timer B


// DMA functions
extern void startDMA(void);                  // Configure and start DMA
extern void stopDMA(void);                   // Stop DMA operation
extern void triggerDMA(void);                // Trigger DMA transfer
extern void set_dma_src_addr(__SFR_FARPTR addr);   // set DMA source address
extern void set_dma_dst_addr(__SFR_FARPTR addr);   // set DMA destination address
extern void triggerDMA0(void);             // Trigger DMA0 transfer
extern void triggerDMA1(void);             // Trigger DMA1 transfer

// Keypad functions
extern void en_keypad_interrupts(void);      // Enable keypad interrupts
extern void disable_keypad_interrupts(void); // Disable keypad interrupts

// LED functions
extern void clear_LEDs(void);                // Turn off all LEDs

// Interrupt handler declarations
extern __interrupt void PBs_handler(void);            // Port 1 interrupt handler
extern __interrupt void keypad_handler(void);         // Port 2 (keypad) interrupt handler
extern __interrupt void TimerA_ISR(void);             // Timer A interrupt handler
extern __interrupt void TimerB_ISR(void);             // Timer B interrupt handler

// LCD configuration

#define LCD_WAIT DelayMs(5)
// LCD control signal macros
#define LCD_EN(a)   (!a ? (LCD_CTL_OUT &= ~LCD_EN_PIN) : (LCD_CTL_OUT |= LCD_EN_PIN))
#define LCD_EN_DIR(a)   (!a ? (LCD_CTL_DIR &= ~LCD_EN_PIN) : (LCD_CTL_DIR |= LCD_EN_PIN))
#define LCD_RS(a)   (!a ? (LCD_CTL_OUT &= ~LCD_RS_PIN) : (LCD_CTL_OUT |= LCD_RS_PIN))
#define LCD_RS_DIR(a)   (!a ? (LCD_CTL_DIR &= ~LCD_RS_PIN) : (LCD_CTL_DIR |= LCD_RS_PIN))
#define LCD_RW(a)   (!a ? (LCD_CTL_OUT &= ~LCD_RW_PIN) : (LCD_CTL_OUT |= LCD_RW_PIN))
#define LCD_RW_DIR(a)   (!a ? (LCD_CTL_DIR &= ~LCD_RW_PIN) : (LCD_CTL_DIR |= LCD_RW_PIN))

#define LCD_DATA_OFFSET 0x04  // Data pin selection offset for 4-bit mode

// LCD mode configuration
#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

// Pin direction definitions
#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

// LCD strobe macro
#define LCD_STROBE_READ(value)  LCD_EN(1), \
                              asm("nop"), asm("nop"), \
                              value=LCD_DATA_READ, \
                              LCD_EN(0)

// LCD command macros
#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)         lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()          lcd_cmd(0x02)
#define cursor_off          lcd_cmd(0x0C)
#define cursor_on           lcd_cmd(0x0F)
#define lcd_function_set    lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line        lcd_cmd(0xC0)

// LCD function declarations
extern void lcd_cmd(unsigned char);        // Send command to LCD
extern void lcd_data(unsigned char);       // Send data to LCD
extern void lcd_puts(const char * s);      // Display string on LCD
extern void lcd_init(void);                // Initialize LCD
extern void lcd_strobe(void);              // LCD strobe function
extern void DelayMs(unsigned int);         // Millisecond delay
extern void DelayUs(unsigned int);         // Microsecond delay




#endif
