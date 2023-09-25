/**
 * @file
 * @brief       Main program file for ECG-HRM
 * @author      Bryan McElvy
 */

// App. Software Functions
#include "DAQ.h"
#include "Debug.h"
#include "LCD.h"
#include "QRS.h"

// Driver Libraries
#include "PLL.h"

int main(void) {}

/**
 * @brief   Interrupt service routine (ISR) for collecting ADC samples.
 *
 */
void ADC0_SS3_Handler(void) {}

/**
 * @brief   Interrupt service routine (ISR) for outputting data to the LCD.
 *
 */
void Timer1A_Handler(void) {}
