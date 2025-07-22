#include  "../header/bsp_msp430x2xx.h"    		// private library - BSP layer FOR SERIES 2 KIT
//#include  "../header/bsp_msp430x4xx.h"    		// private library - BSP layer FOR SERIES 2 KIT
//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // LEDs 8-bit Array Port configuration
  LEDsArrPortSel &= ~0xFF;            // GPIO capability
  LEDsArrPortDir |= 0xFF;             // output dir
  LEDsArrPort = 0x00;				  // clear all LEDs
  
  // Switches Setup
  SWsArrPortSel &= ~0x0F;
  SWsArrPortDir &= ~0x0F;
  
  // PushButtons Setup
  PBsArrPortSel &= ~0x0F;
  PBsArrPortDir &= ~0x0F;
  PBsArrIntEdgeSel |= 0x03;  	     // pull-up mode
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode
  PBsArrIntEn |= 0x0F;
  PBsArrIntPend &= ~0x0F;            // clear pending interrupts 
  
  _BIS_SR(GIE);                     // enable interrupts globally
}
//------------------------------------------------------------------------------------- 
//            PWM configuration 
//-------------------------------------------------------------------------------------
void PWMconfig(void){
    // Configure P2.7 as GPIO output for PWM
    PWM_PORT_SEL &= ~PWM_PIN;  // GPIO capability
    PWM_PORT_DIR |= PWM_PIN;   // Set as output
    PWM_PORT_OUT &= ~PWM_PIN;  // Initialize low
}
//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
         
	
	//write here timers congiguration code
} 
//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
	
	//write here ADC congiguration code
}              

           
             

 
             
             
            
  

