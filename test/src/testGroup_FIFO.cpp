// clang-format off
// NOLINTBEGIN

#include "CppUTest/TestHarness.h"
#include "CppUTest/UtestMacros.h"

extern "C" {
#include "FIFO.h"

#include <stdlib.h>
}

#define FIFO_BUFFER_SIZE    10
#define FIFO_CAPACITY       FIFO_BUFFER_SIZE - 1

/******************************************************************************
SECTIONS
        After Initialization
        After Reset
        After Put
        After Get
        After Flush
        After Peek
        After Reaching Capacity
*******************************************************************************/

/******************************************************************************
After Initialization
*******************************************************************************/

TEST_GROUP(Group_FIFO_AfterInit) {
    volatile FIFO_t * fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };

    void setup() {
        fifo = FIFO_Init(fifoBuffer, FIFO_BUFFER_SIZE);
    } 

    void teardown() {
        FIFO_Reset(fifo);
    } 
};

TEST(Group_FIFO_AfterInit, AfterInit_isNotFull) {
    CHECK_FALSE(FIFO_isFull(fifo));
}

TEST(Group_FIFO_AfterInit, AfterInit_isEmpty) {
    CHECK_TRUE(FIFO_isEmpty(fifo));
}

/******************************************************************************
After Reset
*******************************************************************************/

TEST_GROUP(Group_FIFO_AfterReset) {
    volatile FIFO_t * fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };
    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};
    uint8_t rand_idx;

    void setup() {
        fifo = FIFO_Init(fifoBuffer, FIFO_BUFFER_SIZE);

        // Fill with a random number in range [1, FIFO_CAPACITY]
        rand_idx = 1 + (rand() % (FIFO_CAPACITY - 1));
        for(uint8_t n = 0; n < rand_idx; n++) {
            FIFO_Put(fifo, randArray[n]);
        }
    } 

    void teardown() {} 
};

TEST(Group_FIFO_AfterReset, AfterReset_SizeIsZero) {
    FIFO_Reset(fifo);
    CHECK_TRUE(FIFO_getCurrSize(fifo) == 0);
}

TEST(Group_FIFO_AfterReset, AfterReset_isEmpty) {
    FIFO_Reset(fifo);
    CHECK_TRUE(FIFO_isEmpty(fifo));
}

TEST(Group_FIFO_AfterReset, AfterReset_isNotFull) {
    FIFO_Reset(fifo);
    CHECK_FALSE(FIFO_isFull(fifo));
}

/******************************************************************************
After Put
*******************************************************************************/

TEST_GROUP(Group_FIFO_AfterPut) {
    volatile FIFO_t * fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };
    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};
    uint8_t rand_idx;

    void setup() {
        fifo = FIFO_Init(fifoBuffer, FIFO_BUFFER_SIZE);

        // Fill with a random number in range [1, FIFO_CAPACITY]
        rand_idx = 1 + (rand() % (FIFO_CAPACITY - 1));
        for(uint8_t n = 0; n < rand_idx; n++) {
            FIFO_Put(fifo, randArray[n]);
        }
    } 

    void teardown() {} 
};

TEST(Group_FIFO_AfterPut, AfterPut_SizeIncreases) {
    uint8_t sizeBeforePut = FIFO_getCurrSize(fifo);
    FIFO_Put(fifo, randArray[rand_idx]);
    uint8_t sizeAfterPut = FIFO_getCurrSize(fifo);

    CHECK_EQUAL(sizeBeforePut + 1, sizeAfterPut);
}

TEST(Group_FIFO_AfterPut, AfterPut_isNotEmpty) {
    FIFO_Put(fifo, randArray[rand_idx]);
    CHECK_FALSE(FIFO_isEmpty(fifo));
}

/******************************************************************************
After Get
*******************************************************************************/

TEST_GROUP(Group_FIFO_AfterGet) {
    volatile FIFO_t * fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };
    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};
    uint8_t rand_idx;

    void setup() {
        fifo = FIFO_Init(fifoBuffer, FIFO_BUFFER_SIZE);

        // Fill with a random number in range [1, FIFO_CAPACITY]
        rand_idx = 1 + (rand() % (FIFO_CAPACITY - 1));
        for(uint8_t n = 0; n < rand_idx; n++) {
            FIFO_Put(fifo, randArray[n]);
        }
    } 

    void teardown() {} 
};

TEST(Group_FIFO_AfterGet, AfterGet_SizeDecreases) {
    uint8_t sizeBeforeGet = FIFO_getCurrSize(fifo);
    FIFO_Get(fifo);
    uint8_t sizeAfterGet = FIFO_getCurrSize(fifo);

    CHECK_EQUAL(sizeBeforeGet - 1, sizeAfterGet);
}

/******************************************************************************
After Flush
*******************************************************************************/

TEST_GROUP(Group_FIFO_AfterFlush) {
    volatile FIFO_t * fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };
    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};
    uint32_t outputArray[FIFO_CAPACITY] = { 0 };
    uint8_t rand_idx;

    void setup() {
        fifo = FIFO_Init(fifoBuffer, FIFO_BUFFER_SIZE);

        // Fill with a random number in range [1, FIFO_CAPACITY]
        rand_idx = 1 + (rand() % (FIFO_CAPACITY - 1));
        for(uint8_t n = 0; n < rand_idx; n++) {
            FIFO_Put(fifo, randArray[n]);
        }
    } 

    void teardown() {} 
};

TEST(Group_FIFO_AfterFlush, AfterFlush_isEmpty) {
    FIFO_Flush(fifo, outputArray);
    CHECK_TRUE(FIFO_isEmpty(fifo));
}

TEST(Group_FIFO_AfterFlush, AfterFlush_OutputHasCorrectVals) {
    FIFO_Flush(fifo, outputArray);

    bool areAllCorrect = true;
    for(uint8_t n = 0; n < rand_idx; n++) {
        if (outputArray[n] != randArray[n]) {
            areAllCorrect = false;
            break;
        }
    }

    CHECK_TRUE(areAllCorrect);
}

/******************************************************************************
After Peek
*******************************************************************************/

TEST_GROUP(Group_FIFO_AfterPeek) {
    volatile FIFO_t * fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };
    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};
    uint32_t outputArray[FIFO_CAPACITY] = { 0 };
    uint8_t randNum;

    void setup() {
        fifo = FIFO_Init(fifoBuffer, FIFO_BUFFER_SIZE);

        // Fill with a random number in range [1, FIFO_CAPACITY]
        randNum = 1 + (rand() % (FIFO_CAPACITY - 1));
        for(uint8_t n = 0; n < randNum; n++) {
            FIFO_Put(fifo, randArray[n]);
        }
    } 

    void teardown() {} 
};

TEST(Group_FIFO_AfterPeek, AfterPeekOne_SizeDoesNotChange) {
    uint32_t sizeBeforePeek = FIFO_getCurrSize(fifo);
    FIFO_PeekOne(fifo);
    uint32_t sizeAfterPeek = FIFO_getCurrSize(fifo);
    CHECK_EQUAL(sizeBeforePeek, sizeAfterPeek);
}

TEST(Group_FIFO_AfterPeek, AfterPeekOne_ReturnValIsCorrect) {
    uint32_t peekVal = FIFO_PeekOne(fifo);
    uint32_t getVal = FIFO_Get(fifo);
    CHECK_EQUAL(getVal, peekVal);
}

TEST(Group_FIFO_AfterPeek, AfterPeekAll_SizeDoesNotChange) {
    uint32_t sizeBeforePeek = FIFO_getCurrSize(fifo);
    FIFO_PeekAll(fifo, outputArray);
    uint32_t sizeAfterPeek = FIFO_getCurrSize(fifo);

    CHECK_EQUAL(sizeBeforePeek, sizeAfterPeek);
}

TEST(Group_FIFO_AfterPeek, AfterPeekAll_OutputHasCorrectVals) {
    FIFO_PeekAll(fifo, outputArray);
    bool areAllCorrect = true;
    for(uint8_t n = 0; n < randNum; n++) {
        uint32_t temp_val = FIFO_Get(fifo);
        if(outputArray[n] != temp_val) {
            areAllCorrect = false;
        }
    }

    CHECK_TRUE(areAllCorrect);
}

/******************************************************************************
After Reaching Capacity
*******************************************************************************/

TEST_GROUP(Group_FIFO_AfterReachingCapacity) {
    volatile FIFO_t * fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };
    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};

    void setup() {
        fifo = FIFO_Init(fifoBuffer, FIFO_BUFFER_SIZE);
        for(uint8_t n = 0; n < FIFO_CAPACITY; n++) {
            FIFO_Put(fifo, randArray[n]);
        }
    } 

    void teardown() {} 
};

TEST(Group_FIFO_AfterReachingCapacity, AfterReachingCapacity_isFull) {
    CHECK_TRUE(FIFO_isFull(fifo));
}

TEST(Group_FIFO_AfterReachingCapacity, AfterReachingCapacity_isNotEmpty) {
    CHECK_FALSE(FIFO_isEmpty(fifo));
}

TEST(Group_FIFO_AfterReachingCapacity, AfterReachingCapacity_isNotFullAfterGet) {
    FIFO_Get(fifo);
    CHECK_FALSE(FIFO_isFull(fifo));
}

// NOLINTEND
