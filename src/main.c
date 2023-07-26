/**
 * @file
 * @author  Bryan McElvy
 * @brief   Main program file for ECG-HRM
 */

// Driver Libraries
#include "ADC.h"
#include "ILI9341.h"
#include "PLL.h"

// App. Software Functions
#include "Debug.h"
#include "Filter.h"
#include "QRS.h"
#include "UserCtrl.h"

int main(void) {
    
}

/**
 * @brief   Interrupt service routine (ISR) for the UserCtrl module via GPIO Port F.
 * 
 */
void GPIO_PortF_Handler(void) {

}

/**
 * @brief   Interrupt service routine (ISR) for collecting ADC samples.
 * 
 */
void ADC0_SS3_Handler(void) {

}

/**
 * @brief   Interrupt service routine (ISR) for outputting data to the LCD.
 * 
 */
void Timer1A_Handler(void) {

}
