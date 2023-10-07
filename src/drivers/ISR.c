/**
 * @addtogroup isr
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for interrupt vector handling module.
 */

#include "ISR.h"

/******************************************************************************
Preprocessor Directives
*******************************************************************************/

#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

#define VECTOR_TABLE_BASE_ADDR (uint32_t) 0x00000000
#define VECTOR_TABLE_SIZE      (uint32_t) 155
#define VECTOR_TABLE_ALIGNMENT (uint32_t)(1 << 10)

#define NVIC_EN_BASE_ADDR      (uint32_t) 0xE000E100
#define NVIC_DIS_BASE_ADDR     (uint32_t) 0xE000E180
#define NVIC_PRI_BASE_ADDR     (uint32_t) 0xE000E400
#define NVIC_UNPEND_BASE_ADDR  (uint32_t) 0xE000E280

static void ISR_setStatus(const uint8_t vectorNum, const bool isEnabled);

/******************************************************************************
Global Interrupt Configuration
*******************************************************************************/

static bool interruptsAreEnabled = true;

void ISR_GlobalDisable(void) {
    // NOTE: Does not affect Reset, NMI, or hard faults
    interruptsAreEnabled = false;
    __asm__("   CPSID   I\n\t"                // Set I bit in PRIMASK

            "   BX      LR\n\t"               // Return

    );
}

void ISR_GlobalEnable(void) {
    // NOTE: Does not affect Reset, NMI, or hard faults
    interruptsAreEnabled = true;
    __asm__("   CPSIE   I\n\t"               // Clear I bit in PRIMASK

            "   BX      LR"                  // Return

    );
}

/******************************************************************************
Interrupt Vector Table Configuration
*******************************************************************************/

// Original table, located in flash and defined in `startup_gcc.c`
extern void (*const interruptVectorTable[])(void);

// New table, located in SRAM and defined here
static ISR_t newVectorTable[VECTOR_TABLE_SIZE] __attribute__((aligned(VECTOR_TABLE_ALIGNMENT)));

static bool isTableCopiedToRam = false;

void ISR_InitNewTableInRam(void) {
    Assert(isTableCopiedToRam == false);
    Assert(interruptsAreEnabled == false);

    for(uint32_t idx = 0; idx < VECTOR_TABLE_SIZE; idx++) {
        newVectorTable[idx] = interruptVectorTable[idx];
    }

    NVIC_VTABLE_R = (uint32_t) &newVectorTable;
    isTableCopiedToRam = true;

    return;
}

void ISR_addToIntTable(ISR_t isr, const uint8_t vectorNum) {
    Assert(isTableCopiedToRam == true);
    Assert(interruptsAreEnabled == false);
    Assert(vectorNum >= 16);
    Assert(vectorNum < VECTOR_TABLE_SIZE);

    newVectorTable[vectorNum] = isr;
    return;
}

/******************************************************************************
Individual Interrupt Configuration
*******************************************************************************/

typedef volatile uint32_t * register_t;

void ISR_setPriority(const uint8_t vectorNum, const uint8_t priority) {
    Assert(vectorNum >= 16);
    Assert(vectorNum < VECTOR_TABLE_SIZE);
    Assert(priority <= 7);

    uint8_t interruptBitNum = vectorNum - 16;

    // Determine correct register and assign priority
    uint8_t priorityRegisterNum = (interruptBitNum - (interruptBitNum % 4)) / 4;
    register_t priorityRegisterPtr = (register_t) (NVIC_PRI_BASE_ADDR + (4 * priorityRegisterNum));
    switch((interruptBitNum % 4)) {
        case 0:
            *priorityRegisterPtr |= (priority << 5);
            Assert(*priorityRegisterPtr & (priority << 5));
            break;

        case 1:
            *priorityRegisterPtr |= (priority << 13);
            Assert(*priorityRegisterPtr & (priority << 13));
            break;

        case 2:
            *priorityRegisterPtr |= (priority << 21);
            Assert(*priorityRegisterPtr & (priority << 21));
            break;

        case 3:
            *priorityRegisterPtr |= (priority << 29);
            Assert(*priorityRegisterPtr & (priority << 29));
            break;
    }

    return;
}

static void ISR_setStatus(const uint8_t vectorNum, const bool isEnabled) {
    Assert(vectorNum >= 16);
    Assert(vectorNum < VECTOR_TABLE_SIZE);
    uint32_t interruptBitNum = (uint32_t) (vectorNum - 16);

    // Determine correct register to use
    uint32_t registerNum = 0;
    while(interruptBitNum >= ((registerNum + 1) * 32)) {
        registerNum += 1;
    }
    uint32_t REG_BASE_ADDR = (isEnabled) ? NVIC_EN_BASE_ADDR : NVIC_DIS_BASE_ADDR;
    register_t registerPtr = (register_t) (REG_BASE_ADDR + (4 * registerNum));

    // Enable/disable the ISR
    if(interruptBitNum > 31) {
        interruptBitNum -= (registerNum * 32);
    }
    *registerPtr |= (1 << interruptBitNum);

    return;
}

void ISR_Enable(const uint8_t vectorNum) {
    ISR_setStatus(vectorNum, true);
    return;
}

void ISR_Disable(const uint8_t vectorNum) {
    ISR_setStatus(vectorNum, false);
    return;
}

/******************************************************************************
Individual Interrupt Operations
*******************************************************************************/

void ISR_triggerInterrupt(const uint8_t vectorNum) {
    Assert(vectorNum >= 16);
    Assert(vectorNum < VECTOR_TABLE_SIZE);

    NVIC_SW_TRIG_R = (NVIC_SW_TRIG_R & ~(0xFF)) | (vectorNum - 16);
    return;
}

void ISR_clearPending(const uint8_t vectorNum) {
    Assert(vectorNum >= 16);
    Assert(vectorNum < VECTOR_TABLE_SIZE);
    uint8_t interruptBitNum = vectorNum - 16;

    // Determine correct disable register to use
    uint8_t registerNum = 0;
    while(interruptBitNum >= ((registerNum + 1) * 32)) {
        registerNum += 1;
    }
    interruptBitNum = interruptBitNum - (registerNum * 32);
    register_t registerPtr = (register_t) (NVIC_DIS_BASE_ADDR + (4 * registerNum));

    // Unpend the ISR
    interruptBitNum -= registerNum * 32;
    *registerPtr |= (1 << interruptBitNum);

    return;
}

/** @} */               // isr
