/**
 * @addtogroup  thread
 * @{
 * 
 * @file    isr.c
 * @author  Bryan McElvy
 * @brief   Source code for interrupt service routines (ISRs)
 */

/**
 * @brief   ISR for facilitating user control of program state.
 * 
 * Initialized by GPIO_PF_Interrupt_Init() from the @ref gpio "GPIO" module.
 */
void GPIO_PortF_Handler() {}

/**
 * @brief   ISR for collecting ECG samples @ \f$f_s = 200 [Hz]\f$.
 * 
 * Initialized by SysTick_Interrupt_Init() from the @ref systick "SysTick" module.
 */
void SysTick_Handler() {}

/**
 * @brief   ISR for updating the LCD @ \f$f_s = 30 [Hz]\f$.
 * 
 * Initialized by Timer1A_Init() from the @ref timer "Timer" module.
 */
void Timer1A_Handler() {}

/** @} */
