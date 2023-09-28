/**
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for interrupt vector handling module.
 */

#include "ISR.h"

#include "NewAssert.h"

#include "tm4c123gh6pm.h"

#include <stdint.h>

#define VECTOR_TABLE_BASE_ADDR (uint32_t) 0x00000000
#define NVIC_EN_BASE_ADDR      (uint32_t) 0xE000E100
#define NVIC_DIS_BASE_ADDR     (uint32_t) 0xE000E180
#define NVIC_PRI_BASE_ADDR     (uint32_t) 0xE000E400

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

void ISR_GlobalDisable(void) {
    // NOTE: Does not affect Reset, NMI, or hard faults
    __asm__("   CPSID   I\n\t"                // Set I bit in PRIMASK

            "   BX      LR\n\t"               // Return

    );

    return;
}

void ISR_GlobalEnable(void) {
    // NOTE: Does not affect Reset, NMI, or hard faults
    __asm__("   CPSIE   I\n\t"               // Clear I bit in PRIMASK

            "   BX      LR"                  // Return

    );

    return;
}
