#ifndef _halGPIO_H_
#define _halGPIO_H_

#include "../header/bsp.h"        // Board Support Package
#include "../header/app.h"        // Application layer definitions

// Global variables declaration
extern enum FSMstate state;
extern enum SYSmode lpm_mode;
extern unsigned int REdge1, REdge2;  // For frequency measurement
extern unsigned int seconds;         // For stopwatch
extern unsigned char stopwatch_running;

// System configuration
extern void sysConfig(void);

// General timing functions
extern void delay(unsigned int t);          // Simple delay function
extern void DelayMs(unsigned int ms);       // Millisecond delay
extern void DelayUs(unsigned int us);       // Microsecond delay

// Low-power mode handling
extern void enterLPM(unsigned char LPM_level);
extern void enable_interrupts(void);
extern void disable_interrupts(void);

// LCD functions
extern void lcd_init(void);
extern void lcd_cmd(unsigned char c);
extern void lcd_data(unsigned char c);
extern void lcd_puts(const char* s);
extern void lcd_strobe(void);
extern void write_stopwatch_LCD(unsigned int sec);
extern void clear_LCD_line(unsigned char line);


// Interrupt service routine prototypes
extern __interrupt void PB_handler(void);           // Port 1 ISR (Pushbuttons)
extern __interrupt void TimerA0_ISR(void);          // Timer A0 ISR (Stopwatch tick)
extern __interrupt void TimerA1_ISR(void);          // Timer A1 ISR (Capture events)
extern __interrupt void SW_handler(void);            // ADC10 ISR
extern __interrupt void ADC10_ISR(void);            // ADC10 ISR

// LCD definitions
#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

// LCD pin configurations (based on BSP definitions)
#define LCD_EN(a)   (!a ? (P2OUT&=~0X20) : (P2OUT|=0X20)) // P2.5 is LCD enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~0X20) : (P2DIR|=0X20))

#define LCD_RS(a)   (!a ? (P2OUT&=~0X40) : (P2OUT|=0X40)) // P2.6 is LCD RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~0X40) : (P2DIR|=0X40))

#define LCD_RW(a)   (!a ? (P2OUT&=~0X80) : (P2OUT|=0X80)) // P2.7 is LCD RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~0X80) : (P2DIR|=0X80))

#define LCD_DATA_OFFSET 0x04 // Data pin selection offset for 4-bit mode (P1.4-P1.7)

// LCD mode configuration
#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

// Pin direction configuration
#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

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
#define lcd_function_set    lcd_cmd(0x3C) // 8bit, two lines, 5x10 dots
#define lcd_new_line        lcd_cmd(0xC0)

#endif
