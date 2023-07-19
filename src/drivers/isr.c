/**
 * @file    isr.c
 * @author  Bryan McElvy
 * @brief   Source code for interrupt service routines (ISRs)
 * 
 * @addtogroup  thread
 * @{
 */

/**
 * @brief   ISR for facilitating user control of program state.
 * 
 */
void GPIO_PortF_Handler() {}

/**
 * @brief   ISR for collecting ECG samples @ f_s = 200 [Hz]
 * 
 */
void SysTick_Handler() {}

/**
 * @brief   ISR for updating the LCD @ f_s = 30 [Hz]
 * 
 */
void Timer1A_Handler() {}

/** @} */
