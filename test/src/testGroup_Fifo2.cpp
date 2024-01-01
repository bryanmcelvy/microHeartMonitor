// clang-format off
// NOLINTBEGIN

#include "CppUTest/TestHarness.h"
#include "CppUTest/UtestMacros.h"

extern "C" {
#include "Fifo2.h"

#include <stdlib.h>
}

#define FIFO_BUFFER_SIZE    (20)
#define FIFO_CAPACITY       (FIFO_BUFFER_SIZE - 1)

/******************************************************************************
Given FIFO is Empty...
******************************************************************************/

TEST_GROUP(GivenFifoIsEmpty) {
    volatile Fifo_t fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };

    void setup() {
        srand(time(0));
        fifo = Fifo_DynCreate(FIFO_BUFFER_SIZE, sizeof(uint32_t));
    } 

    void teardown() {
        Fifo_DynDestroy(fifo);
    } 
};

/* When Put Is Called... */

TEST(GivenFifoIsEmpty, WhenPutIsCalled_ThenStatusIsTrue) {
    uint32_t num = rand() % UINT32_MAX;
    FifoStatus_e status = Fifo_Put(fifo, &num);
    CHECK_TRUE(status);
}

TEST(GivenFifoIsEmpty, WhenPutIsCalled_ThenSizeIsOne) {
    uint32_t num = rand() % UINT32_MAX;
    Fifo_Put(fifo, &num);
    uint32_t size = Fifo_getCurrSize(fifo);
    LONGS_EQUAL(1, size);
}

/* When Get Is Called... */

TEST(GivenFifoIsEmpty, WhenGetIsCalled_ThenStatusIsFalse) {
    uint32_t num = rand() % UINT32_MAX;
    FifoStatus_e status = Fifo_Get(fifo, &num);
    CHECK_FALSE(status);
}

/* When Flush Is Called... */

TEST(GivenFifoIsEmpty, WhenFlushIsCalled_ThenStatusIsFalse) {
    uint32_t numArray[5] = {0};
    FifoStatus_e status = Fifo_Flush(fifo, numArray);
    CHECK_FALSE(status);
}

/* When Peek(All) Is Called... */

TEST(GivenFifoIsEmpty, WhenPeekIsCalled_ThenStatusIsFalse) {
    uint32_t num = rand() % UINT32_MAX;
    FifoStatus_e status = Fifo_PeekOne(fifo, &num);
    CHECK_FALSE(status);
}

TEST(GivenFifoIsEmpty, WhenPeekAllIsCalled_ThenStatusIsFalse) {
    uint32_t numArray[5] = {0};
    FifoStatus_e status = Fifo_PeekAll(fifo, numArray);
    CHECK_FALSE(status);
}

/* When Checked... */

TEST(GivenFifoIsEmpty, WhenCheckedIfFull_ThenReturnValIsFalse) {
    CHECK_FALSE(Fifo_isFull(fifo));
}

TEST(GivenFifoIsEmpty, WhenCheckedIfEmpty_ThenReturnValIsTrue) {
    CHECK_TRUE(Fifo_isEmpty(fifo));
}

TEST(GivenFifoIsEmpty, WhenSizeIsChecked_ThenReturnValIsZero) {
    uint32_t size = Fifo_getCurrSize(fifo);
    LONGS_EQUAL(0, size);
}

/******************************************************************************
Given FIFO is Full...
******************************************************************************/

TEST_GROUP(GivenFifoIsFull) {
    volatile Fifo_t fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };

    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};

    void setup() {
        srand(time(0));

        fifo = Fifo_DynCreate(FIFO_BUFFER_SIZE, sizeof(uint32_t));
        for(uint32_t n = 0; n < FIFO_CAPACITY; n++) {
            Fifo_Put(fifo, &randArray[n]);
        }
    } 

    void teardown() {
        Fifo_DynDestroy(fifo);
    } 
};

/* When Put Is Called... */

TEST(GivenFifoIsFull, WhenPutIsCalled_ThenStatusIsFalse) {
    uint32_t num = rand() % UINT32_MAX;
    FifoStatus_e status = Fifo_Put(fifo, &num);
    CHECK_FALSE(status);
}

/* When Get Is Called... */

TEST(GivenFifoIsFull, WhenGetIsCalled_ThenStatusIsTrue) {
    uint32_t num;
    FifoStatus_e status = Fifo_Get(fifo, &num);
    CHECK_TRUE(status);
}

TEST(GivenFifoIsFull, WhenGetIsCalled_ThenSizeIsCorrect) {
    uint32_t num;
    Fifo_Get(fifo, &num);
    uint32_t size = Fifo_getCurrSize(fifo);
    LONGS_EQUAL(FIFO_CAPACITY - 1, size);
}

TEST(GivenFifoIsFull, WhenGetIsCalled_ThenValIsCorrect) {
    uint32_t num;
    Fifo_Get(fifo, &num);
    LONGS_EQUAL(randArray[0], num);
}

/* When Flush Is Called... */

TEST(GivenFifoIsFull, WhenFlushIsCalled_ThenValuesAreCorrect) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_Flush(fifo, numArray);

    bool status = true;
    for(uint32_t n = 0; n < FIFO_CAPACITY; n++) {
        if(numArray[n] != randArray[n]) {
            status = false;
            break;
        }
    }

    CHECK_TRUE(status);
}

TEST(GivenFifoIsFull, WhenFlushIsCalled_ThenFifoIsEmpty) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_Flush(fifo, numArray);
    CHECK_TRUE(Fifo_isEmpty(fifo));
}

TEST(GivenFifoIsFull, WhenFlushIsCalled_ThenSizeIsZero) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_Flush(fifo, numArray);
    CHECK_EQUAL(0, Fifo_getCurrSize(fifo));
}

TEST(GivenFifoIsFull, WhenFlushIsCalled_ThenStatusIsTrue) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    FifoStatus_e status = Fifo_Flush(fifo, numArray);
    CHECK_TRUE(status);
}

/* When Checked... */

TEST(GivenFifoIsFull, WhenCheckedIfEmpty_ThenReturnValIsFalse) {
    CHECK_FALSE(Fifo_isEmpty(fifo));
}

TEST(GivenFifoIsFull, WhenCheckedIfFull_ThenReturnValIsTrue) {
    CHECK_TRUE(Fifo_isFull(fifo));
}

TEST(GivenFifoIsFull, WhenSizeIsChecked_ThenReturnValIsCorrect) {
    LONGS_EQUAL(FIFO_CAPACITY, Fifo_getCurrSize(fifo));
}

/******************************************************************************
Given FIFO is Not Empty or Full
******************************************************************************/

TEST_GROUP(GivenFifoIsNotEmptyOrFull) {
    volatile Fifo_t fifo;
    volatile uint32_t fifoBuffer[FIFO_BUFFER_SIZE] = { 0 };
    uint32_t randArray[FIFO_CAPACITY] = {29, 81, 73, 79, 2, 40, 21, 60, 93};
    uint8_t randNum;

    void setup() {
        fifo = Fifo_DynCreate(FIFO_BUFFER_SIZE, sizeof(uint32_t));

        // Fill with a random number in range [1, FIFO_CAPACITY]
        randNum = 1 + (rand() % (FIFO_CAPACITY - 1));
        for(uint8_t n = 0; n < randNum; n++) {
            Fifo_Put(fifo, &randArray[n]);
        }
    }

    void teardown() {
        Fifo_DynDestroy(fifo);
    } 
};

/* When Put Is Called... */

TEST(GivenFifoIsNotEmptyOrFull, WhenPutIsCalled_ThenStatusIsTrue) {
    uint32_t num = rand() % UINT32_MAX;
    FifoStatus_e status = Fifo_Put(fifo, &num);
    CHECK_TRUE(status);
}

TEST(GivenFifoIsNotEmptyOrFull, WhenPutIsCalled_SizeIncreases) {
    uint32_t currSize = Fifo_getCurrSize(fifo);

    uint32_t num = rand() % UINT32_MAX;
    Fifo_Put(fifo, &num);
    uint32_t newSize = Fifo_getCurrSize(fifo);

    LONGS_EQUAL(currSize + 1, newSize);
}

/* When Get Is Called... */

TEST(GivenFifoIsNotEmptyOrFull, WhenGetIsCalled_ThenStatusIsTrue) {
    uint32_t num;
    FifoStatus_e status = Fifo_Get(fifo, &num);
    CHECK_TRUE(status);
}

TEST(GivenFifoIsNotEmptyOrFull, WhenGetIsCalled_SizeDecreases) {
    uint32_t currSize = Fifo_getCurrSize(fifo);
    uint32_t num;
    Fifo_Get(fifo, &num);
    uint32_t newSize = Fifo_getCurrSize(fifo);

    LONGS_EQUAL(currSize - 1, newSize);
}

/* When Flush Is Called... */

TEST(GivenFifoIsNotEmptyOrFull, WhenFlushIsCalled_ThenValuesAreCorrect) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_Flush(fifo, numArray);

    bool status = true;
    for(uint32_t n = 0; n < randNum; n++) {
        if(numArray[n] != randArray[n]) {
            status = false;
            break;
        }
    }

    CHECK_TRUE(status);
}

TEST(GivenFifoIsNotEmptyOrFull, WhenFlushIsCalled_ThenFifoIsEmpty) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_Flush(fifo, numArray);
    CHECK_TRUE(Fifo_isEmpty(fifo));
}

TEST(GivenFifoIsNotEmptyOrFull, WhenFlushIsCalled_ThenSizeIsZero) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_Flush(fifo, numArray);
    CHECK_EQUAL(0, Fifo_getCurrSize(fifo));
}

TEST(GivenFifoIsNotEmptyOrFull, WhenFlushIsCalled_ThenStatusIsTrue) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    FifoStatus_e status = Fifo_Flush(fifo, numArray);
    CHECK_TRUE(status);
}

/* When Peek(All) Is Called... */

TEST(GivenFifoIsNotEmptyOrFull, WhenPeekIsCalled_ThenStatusIsTrue) {
    uint32_t num;
    FifoStatus_e status = Fifo_PeekOne(fifo, &num);
    CHECK_TRUE(status);
}

TEST(GivenFifoIsNotEmptyOrFull, WhenPeekIsCalled_SizeDoesNotChange) {
    uint32_t currSize = Fifo_getCurrSize(fifo);

    uint32_t num;
    Fifo_PeekOne(fifo, &num);
    uint32_t newSize = Fifo_getCurrSize(fifo);

    LONGS_EQUAL(currSize, newSize);
}

TEST(GivenFifoIsNotEmptyOrFull, WhenPeekAllIsCalled_ThenValuesAreCorrect) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_PeekAll(fifo, numArray);

    bool status = true;
    for(uint32_t n = 0; n < randNum; n++) {
        if(numArray[n] != randArray[n]) {
            status = false;
            break;
        }
    }

    CHECK_TRUE(status);
}

TEST(GivenFifoIsNotEmptyOrFull, WhenPeekAllIsCalled_ThenStatusIsTrue) {
    uint32_t numArray[FIFO_CAPACITY] = {0};
    FifoStatus_e status = Fifo_PeekAll(fifo, numArray);
    CHECK_TRUE(status);
}

TEST(GivenFifoIsNotEmptyOrFull, WhenPeekAllIsCalled_SizeDoesNotChange) {
    uint32_t currSize = Fifo_getCurrSize(fifo);

    uint32_t numArray[FIFO_CAPACITY] = {0};
    Fifo_PeekAll(fifo, numArray);
    uint32_t newSize = Fifo_getCurrSize(fifo);

    LONGS_EQUAL(currSize, newSize);
}

/* When Checked... */

TEST(GivenFifoIsNotEmptyOrFull, WhenCheckedIfEmpty_ThenReturnValIsFalse) {
    CHECK_FALSE(Fifo_isEmpty(fifo));
}

TEST(GivenFifoIsNotEmptyOrFull, WhenCheckedIfFull_ThenReturnValIsFalse) {
    CHECK_FALSE(Fifo_isFull(fifo));
}

TEST(GivenFifoIsNotEmptyOrFull, WhenSizeIsChecked_ThenReturnValIsCorrect) {
    LONGS_EQUAL(randNum, Fifo_getCurrSize(fifo));
}

// NOLINTEND
