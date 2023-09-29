/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for interrupt vector handling module.
 */

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "ISR.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

#define VECTOR_TABLE_BASE_ADDR (uint32_t) 0x00000000
#define VECTOR_TABLE_SIZE      (uint32_t) 155
#define VECTOR_TABLE_ALIGNMENT (uint32_t)(1 << 10)

#define NVIC_EN_BASE_ADDR      (uint32_t) 0xE000E100
#define NVIC_DIS_BASE_ADDR     (uint32_t) 0xE000E180
#define NVIC_PRI_BASE_ADDR     (uint32_t) 0xE000E400

/******************************************************************************
Global Interrupt Configuration
*******************************************************************************/

static bool interruptsAreEnabled = true;

void ISR_GlobalDisable(void) {
    // NOTE: Does not affect Reset, NMI, or hard faults
    __asm__("   CPSID   I\n\t"                // Set I bit in PRIMASK

            "   BX      LR\n\t"               // Return

    );
    interruptsAreEnabled = false;
    return;
}

void ISR_GlobalEnable(void) {
    // NOTE: Does not affect Reset, NMI, or hard faults
    __asm__("   CPSIE   I\n\t"               // Clear I bit in PRIMASK

            "   BX      LR"                  // Return

    );
    interruptsAreEnabled = true;
    return;
}

/******************************************************************************
Interrupt Vector Table Configuration
*******************************************************************************/

/// @var interruptVectorTable
/// original table, located in flash and defined in `startup_gcc.c`
extern void (*const interruptVectorTable[])(void);

/// @var newVectorTable
/// new table, located in SRAM and defined here
__attribute__((aligned(VECTOR_TABLE_ALIGNMENT))) static ISR_t newVectorTable[VECTOR_TABLE_SIZE];

static bool isTableCopiedToRam = false;

void ISR_InitNewTableInRam(void) {
    Assert(isTableCopiedToRam == false);
    Assert(interruptsAreEnabled == false);

    for(uint32_t idx = 0; idx < VECTOR_TABLE_SIZE; idx++) {
        newVectorTable[idx] = interruptVectorTable[idx];
    }

    NVIC_VTABLE_R = ((uint32_t) &newVectorTable) << 10;
    isTableCopiedToRam = true;

    return;
}

void ISR_addToIntTable(ISR_t isr, const uint8_t vectorNum) {
    Assert(isTableCopiedToRam == true);
    Assert(interruptsAreEnabled == false);
    Assert((vectorNum >= 16) && (vectorNum <= 154));

    newVectorTable[vectorNum] = isr;
    return;
}

/******************************************************************************
Individual Interrupt Configuration
*******************************************************************************/

/// @typedef volatile uint32_t * register_t
typedef volatile uint32_t * register_t;

void ISR_setPriority(const uint8_t vectorNum, const uint8_t priority) {
    Assert((vectorNum >= 16) && (vectorNum <= 154));
    Assert(priority <= 7);

    uint8_t interruptBitNumber = vectorNum - 16;

    // Determine correct register and assign priority
    uint8_t priorityRegisterNum = (interruptBitNumber - (interruptBitNumber % 4)) / 4;
    register_t priorityRegisterPtr = (register_t) (NVIC_PRI_BASE_ADDR + (4 * priorityRegisterNum));
    switch((interruptBitNumber % 4)) {
        case 0:
            *priorityRegisterPtr |= (priority << 5);
            break;
        case 1:
            *priorityRegisterPtr |= (priority << 13);
            break;

        case 2:
            *priorityRegisterPtr |= (priority << 21);
            break;

        case 3:
            *priorityRegisterPtr |= (priority << 29);
            break;
    }

    return;
}

void ISR_Enable(const uint8_t vectorNum) {
    Assert((vectorNum >= 16) && (vectorNum <= 154));
    uint8_t interruptBitNumber = vectorNum - 16;

    // Determine correct enable register to use
    uint8_t enableRegisterNum = 0;
    while(interruptBitNumber >= ((enableRegisterNum + 1) * 32)) {
        enableRegisterNum += 1;
    }
    register_t enableRegisterPtr = (register_t) (NVIC_EN_BASE_ADDR + (4 * enableRegisterNum));

    // Enable the ISR
    *enableRegisterPtr |= (1 << interruptBitNumber);

    return;
}

void ISR_Disable(const uint8_t vectorNum) {
    Assert((vectorNum >= 16) && (vectorNum <= 154));
    uint8_t interruptBitNumber = vectorNum - 16;

    // Determine correct disable register to use
    uint8_t disableRegNum = 0;
    while(interruptBitNumber >= ((disableRegNum + 1) * 32)) {
        disableRegNum += 1;
    }
    register_t disableRegPtr = (register_t) (NVIC_DIS_BASE_ADDR + (4 * disableRegNum));

    // Disable the ISR
    *disableRegPtr |= (1 << interruptBitNumber);

    return;
}
