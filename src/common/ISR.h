/**
 * @file
 * @author  Bryan McElvy
 * @brief   Module for configuring interrupt service routines (ISRs).
 */

#ifndef ISR_H
#define ISR_H

#include <stdint.h>

/******************************************************************************
Global Interrupt Configuration
*******************************************************************************/

/**
 * @brief                   Disable all interrupts globally.
 * @sa                      ISR_GlobalEnable()
 */
void ISR_GlobalDisable(void);

/**
 * @brief                   Enable all interrupts globally.
 * @sa                      ISR_GlobalDisable()
 */
void ISR_GlobalEnable(void);

/******************************************************************************
Interrupt Vector Table Configuration
*******************************************************************************/

/**
 * @brief                   Relocate the vector table to RAM.
 *
 * @pre                     Call this after disabling interrupts globally.
 * @post                    The vector table is now located in RAM, allowing
 *                          the ISRs listed in the startup file to be replaced.
 *
 * @sa                      ISR_GlobalDisable(), ISR_addToIntTable()
 */
void ISR_InitNewTableInRam(void);

/// @typedef    void (*ISR_t)(void)
/// @brief      Type definition for function pointers representing ISRs.
typedef void (*ISR_t)(void);

/**
 * @brief                   Add an ISR to the interrupt table.
 *
 * @pre                     Initialize a new vector table in RAM before calling this function.
 *
 * @param[in] isr           Name of the ISR to add.
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @post                    The ISR is now added to the vector table and available to be called.
 *
 * @sa                      ISR_relocateIntTableToRam()
 */
void ISR_addToIntTable(ISR_t isr, const uint8_t vectorNum);

/******************************************************************************
Individual Interrupt Configuration
*******************************************************************************/

/**
 * @brief                   Set the priority for an interrupt.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 * @param[in] priority      Priority to assign. Highest priority is 0, lowest is 7.
 */
void ISR_setPriority(const uint8_t vectorNum, const uint8_t priority);

/**
 * @brief                   Enable an interrupt in the NVIC.
 *
 * @pre                     If needed, set the interrupt's priority (default 0, or highest
 *                          priority) before calling this.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @sa                      ISR_setPriority(), ISR_Disable()
 */
void ISR_Enable(const uint8_t vectorNum);

/**
 * @brief                   Disable an interrupt in the NVIC.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @sa                      ISR_Enable()
 */
void ISR_Disable(const uint8_t vectorNum);

/******************************************************************************
Individual Interrupt Operations
*******************************************************************************/

/**
 * @brief                   Generate a software-generated interrupt (SGI).
 *
 * @pre                     Enable the ISR (and set priority as needed) for calling this.
 * @pre                     Enable all interrupts before calling this.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @post                    The ISR should trigger once any higher priority ISRs return.
 *
 * @sa                      ISR_clearPending()
 */
void ISR_triggerInterrupt(const uint8_t vectorNum);

/**
 * @brief                   Clear an ISR's pending bit.
 *
 * @pre                     This should be called during the ISR for an SGI.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @post                    The ISR should not trigger again until re-activated.
 *
 * @sa                      ISR_triggerInterrupt()
 */
void ISR_clearPending(const uint8_t vectorNum);

#endif               // ISR_H
