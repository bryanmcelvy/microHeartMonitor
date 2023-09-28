/**
 * @file
 * @author  Bryan McElvy
 * @brief   Module for configuring interrupt service routines.
 */

#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef void (*ISR_t)(void);

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
 */
void ISR_Enable(const uint8_t vectorNum);

/**
 * @brief                   Disable an interrupt.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 */
void ISR_Disable(const uint8_t vectorNum);

/// @brief Disable all interrupts.
void ISR_GlobalDisable(void);

/// @brief Enable all interrupts.
void ISR_GlobalEnable(void);

#endif               // ISR_H
