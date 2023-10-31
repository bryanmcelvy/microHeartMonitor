/**
 * @addtogroup isr
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for interrupt service routine (ISR) configuration module.
 */

#ifndef ISR_H
#define ISR_H

/*******************************************************************************
SECTIONS
        Global Interrupt Configuration
        Interrupt Vector Table Configuration
        Individual Interrupts
********************************************************************************/

#include <stdint.h>

/******************************************************************************
Global Interrupt Configuration
*******************************************************************************/
/** @name Global Interrupt Configuration */               /// @{

/**
 * @brief                   Disable all interrupts globally.
 * @note                    Does not affect Reset, NMI, or hard faults.
 * @see                     ISR_GlobalEnable()
 */
void ISR_GlobalDisable(void);

/**
 * @brief                   Enable all interrupts globally.
 * @note                    Does not affect Reset, NMI, or hard faults.
 * @see                     ISR_GlobalDisable()
 */
void ISR_GlobalEnable(void);

/** @} */               // Global Interrupt Configuration

/******************************************************************************
Interrupt Vector Table Configuration
*******************************************************************************/
/** @name Interrupt Vector Table Configuration */               /// @{

/**
 * @brief                   Relocate the vector table to RAM.
 *
 * @pre                     Disable interrupts globally before calling this.
 * @post                    The vector table is now located in RAM, allowing
 *                          the ISRs listed in the startup file to be replaced.
 *
 * @see                     ISR_GlobalDisable(), ISR_addToIntTable()
 */
void ISR_InitNewTableInRam(void);

/**
 * @typedef                 void (*ISR_t)(void)
 * @brief                   Interrupt service routine (ISR) function pointers.
 */
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
 * @see                     ISR_InitNewTableInRam()
 */
void ISR_addToIntTable(ISR_t isr, const uint8_t vectorNum);

/** @} */               // Interrupt Vector Table Configuration

/******************************************************************************
Individual Interrupt Configuration
*******************************************************************************/
/** @name Individual Interrupt Configuration */               /// @{

/**
 * @brief                   Set the priority for an interrupt.
 *
 * @pre                     Disable the interrupt before adjusting its priority.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @param[in] priority      Priority to assign. Highest priority is 0, lowest is 7.
 *
 * @post                    The interrupt's priority has now been changed in the NVIC.
 *
 * @see                     ISR_Disable()
 */
void ISR_setPriority(const uint8_t vectorNum, const uint8_t priority);

/**
 * @brief                   Enable an interrupt in the NVIC.
 *
 * @pre                     If needed, add the interrupt to the vector table.
 * @pre                     If needed, set the interrupt's priority (default 0, or highest
 *                          priority) before calling this.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @post                    The interrupt is now enabled in the NVIC.
 *
 * @see                     ISR_addToIntTable(), ISR_setPriority(), ISR_Disable()
 */
void ISR_Enable(const uint8_t vectorNum);

/**
 * @brief                   Disable an interrupt in the NVIC.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @post                    The interrupt is now disabled in the NVIC.
 *
 * @see                     ISR_Enable()
 */
void ISR_Disable(const uint8_t vectorNum);

/**
 * @brief                   Generate a software-generated interrupt (SGI).
 *
 * @pre                     Enable the ISR (and set priority as needed).
 * @pre                     Enable all interrupts.
 *
 * @param[in] vectorNum     ISR's vector number (i.e. offset from the top of the table).
 *                          Should be in range `[16, 154]`.
 *
 * @post                    The ISR should trigger once any higher priority ISRs return.
 *
 * @see                     ISR_clearPending()
 */
void ISR_triggerInterrupt(const uint8_t vectorNum);

/** @} */               // Individual Interrupt Configuration

#endif                  // ISR_H

/** @} */               // isr
