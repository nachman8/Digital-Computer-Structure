#ifndef _halGPIO_H_
#define _halGPIO_H_

#ifdef __MSP430FG4619__
#include  "../header/bsp_msp430x4xx.h"
#else
#include  "../header/bsp_msp430x2xx.h"
#endif

#include  "../header/app.h"    		// private library - APP layer


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern unsigned int X_flag;
extern char timerInput[6];
extern char sentence[];
extern int k;
extern int print_negev_flag;
extern char ascii_char;
extern int ascii_display_flag;

extern void sysConfig(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void startTimerA0();
extern void finishTimerA0();
extern void enableTimerA1_bzr();
extern void disableTimerA1_bzr();
extern void startTimerA1();
extern void finishTimerA1();
extern void startADC10();
extern void finishADC10();
extern void enable_UARTTX_interrupts();

extern __interrupt void PBs_handler(void);

#endif

// #define CHECKBUSY    1  // using this define, only if we want to read from LCD

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)   (!a ? (LCD_CTL_OUT&=~LCD_EN_PIN) : (LCD_CTL_OUT|=LCD_EN_PIN)) // P2.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (LCD_CTL_DIR&=~LCD_EN_PIN) : (LCD_CTL_DIR|=LCD_EN_PIN)) // P2.5 pin direction

#define LCD_RS(a)   (!a ? (LCD_CTL_OUT&=~LCD_RS_PIN) : (LCD_CTL_OUT|=LCD_RS_PIN)) // P2.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (LCD_CTL_DIR&=~LCD_RS_PIN) : (LCD_CTL_DIR|=LCD_RS_PIN)) // P2.6 pin direction

#define LCD_RW(a)   (!a ? (LCD_CTL_OUT&=~LCD_RW_PIN) : (LCD_CTL_OUT|=LCD_RW_PIN)) // P2.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (LCD_CTL_DIR&=~LCD_RW_PIN) : (LCD_CTL_DIR|=LCD_RW_PIN)) // P2.7 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset


/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/
#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nop"), asm("nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

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

extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
extern void clear_RGB();
extern void assign_RGB_value(unsigned int value);
extern int get_ADC_value();
extern void enable_UARTRX_interrupts();
/*
 *  Delay functions for HI-TECH C on the PIC18
 *
 *  Functions available:
 *      DelayUs(x)  Delay specified number of microseconds
 *      DelayMs(x)  Delay specified number of milliseconds
*/
