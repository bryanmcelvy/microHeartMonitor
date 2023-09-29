/**
 * @file
 * @author  Bryan McElvy
 * @brief   Module for configuring interrupt service routines (ISRs).
 */

#ifndef ISR_H
#define ISR_H

#include <stdint.h>

/******************************************************************************
Interrupt Vector Table
*******************************************************************************/

/**
 * @brief                   Relocate the vector table to RAM.
 *                          Call this at or near the beginning of an application,
 *                          and before adding any ISRs to the table.
 *
 * @sa                      ISR_addToIntTable()
 */
void ISR_InitNewTableInRam(void);

typedef void (*ISR_t)(void);

/**
 * @brief                   Add an ISR to the interrupt table.
 *                          Call this after relocating the table to RAM.
 *
 * @param[in] isr           Name of the ISR to add.
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @sa                      ISR_relocateIntTableToRam().
 */
void ISR_addToIntTable(ISR_t isr, const uint8_t vectorNum);

/******************************************************************************
Interrupt Configuration
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
 * @brief                   Enable an interrupt.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @sa                      ISR_Disable()
 */
void ISR_Enable(const uint8_t vectorNum);

/**
 * @brief                   Disable an interrupt.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @sa                      ISR_Enable()
 */
void ISR_Disable(const uint8_t vectorNum);

/// @brief                  Enable all interrupts.
/// @sa                     ISR_GlobalDisable()
void ISR_GlobalEnable(void);

/// @brief                  Disable all interrupts.
/// @sa                     ISR_GlobalEnable()
void ISR_GlobalDisable(void);

#endif               // ISR_H
