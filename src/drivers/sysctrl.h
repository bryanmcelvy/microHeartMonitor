/**
 * @addtogroup sysctrl
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Header file for System Control module.
 */

#ifndef SYSCTRL_H
#define SYSCTRL_H

#include <stdbool.h>
#include <stdint-gcc.h>

/******************************************************************************
Phase-Locked Loop (PLL)
*******************************************************************************/

enum PLL_DIVISOR_FREQUENCIES {
    SYSCTRL_PLL_80MHZ = 5,
    SYSCTRL_PLL_66_7MHZ = 6,
    SYSCTRL_PLL_50MHZ = 8,
    SYSCTRL_PLL_40MHZ = 10,
    SYSCTRL_PLL_20MHZ = 20,
    SYSCTRL_PLL_10MHZ = 40,
    SYSCTRL_PLL_8MHZ = 50,
    SYSCTRL_PLL_5MHZ = 80,
    SYSCTRL_PLL_4MHZ = 100,
};

/**
 * @brief                       Initialize the phase-locked loop (PLL) to increase
 *                              the clock frequency.
 *
 * @param[in] divisor           7-bit diviser to use. Should be 5, 6, or in range [8, 128].
 *
 * @post                        The phase-locked loop is initialized, increasing the system clock
 *                              frequency to be equal to (400[MHz] / divisor).
 *
 * @see                         SysCtrl_getClkFreq_Hz()
 */
void SysCtrl_PLL_Init(uint8_t divisor);

/**
 * @brief                       Return the current clock frequency in `[Hz]`.
 *
 * @param[out] freq             Current clock frequency.
 *                              Value is 16[MHz] if the PLL isn't being used.
 *
 * @see                         SysCtrl_PLL_Init()
 */
uint32_t SysCtrl_getClkFreq_Hz(void);

/******************************************************************************
Clock Gating Control
*******************************************************************************/

typedef enum {
    RUN,                               ///< run mode
    SLEEP,                             ///< sleep mode
    DEEP_SLEEP                         ///< deep sleep mode
} SysCtrlClkMode_e;

typedef enum {
    ADC,                               ///< Analog-to-Digital Converter
    DMA,                               ///< Direct Memory Access
    GPIO,                              ///< General-Purpose Input/Output
    I2C,                               ///< Inter-Integrated Circuit
    PWM,                               ///< Pulse Width Modulator
    SSI,                               ///< Synchronous Serial Interface
    TIMER,                             ///< General-Purpose Timer
    UART,                              ///< Universal Asynchronous Receiver/Transmitter
} SysCtrlPeripheral_e;

typedef enum {
    SYSCTRL_CLK_OFF = 0,               ///< turn clock `OFF`
    SYSCTRL_CLK_ON = 1                 ///< turn clock `ON`
} SysCtrlClkStatus_e;

/**
 * @brief                       Configure the clock for a given peripheral in run mode,
 *                              sleep mode, or deep-sleep mode.
 *
 * @param[in] mode              Mode to configure peripheral for. (See @ref SysCtrlClkMode_e)
 * @param[in] peripheral        Peripheral to configure clock for. (See @ref SysCtrlPeripheral_e)
 * @param[in] num               The number for the peripheral.
 * @param[in] status            The desired status for the peripheral.
 *                              (See @ref SysCtrlClkStatus_e)
 *
 * @post                        If status == `ON`, the peripheral is turned on and ready to access.
 * @post                        If status == `OFF`, the peripheral is turned off.
 */
void SysCtrl_configPeripheralClk(SysCtrlClkMode_e mode, SysCtrlPeripheral_e peripheral, uint8_t num,
                                 SysCtrlClkStatus_e status);

/******************************************************************************
Sleep Mode
*******************************************************************************/

typedef enum {
    SYSCTRL_CFG_WAKE_ON_ENABLED,                ///< only enabled interrupts trigger wake up event
    SYSCTRL_CFG_WAKE_ON_PEND,                   ///< all pending interrupts trigger wake up event
    SYSCTRL_CFG_NORMAL_SLEEP,                   ///< use sleep mode for low power mode
    SYSCTRL_CFG_DEEP_SLEEP,                     ///< use deep sleep mode for low power mode
    SYSCTRL_CFG_NO_SLEEP_ON_EXIT,               ///< do not sleep when all int. handlers are done
    SYSCTRL_CFG_SLEEP_ON_EXIT,                  ///< sleep once all interrupt handlers are done
} SysCtrlSleepConfig_e;

/**
 * @brief                       Configure sleep mode.
 *
 * @param[in] param             Parameter to set.
 * @post                        See @ref SysCtrlSleepConfig_e.
 *
 * @see                         SysCtrl_enterSleepMode()
 */
void SysCtrl_configSleepMode(SysCtrlSleepConfig_e param);

typedef enum {
    WAIT_FOR_INT,                 ///< sleep until an interrupt occurs
    WAIT_FOR_EVENT,               ///< sleep until an event occurs
} SysCtrlSleepModes_e;

/**
 * @brief                       Enter sleep mode.
 *
 * @param[in] param             Parameter to set.
 * @post                        See @ref SysCtrlSleepModes_e.
 *
 * @see                         SysCtrl_configSleepMode()
 */
void SysCtrl_enterSleepMode(SysCtrlSleepModes_e sleepMode);

/******************************************************************************/

#endif

/** @} */               // sysctrl
