// clang-format off

#include <stdint.h>

/******************************************************************************
SECTIONS
    Space/Punctuation
    Numbers
    Uppercase Alphabet
    Lowercase Alphabet
    Font Array
*******************************************************************************/

/******************************************************************************
Space/Punctuation
*******************************************************************************/

static const uint8_t FONT_SPACE[8] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

static const uint8_t FONT_PERIOD[8] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x04,
    0x04
};

static const uint8_t FONT_COLON[8] = {
    0x00,
    0x04,
    0x00,
    0x00,
    0x00,
    0x04,
    0x00,
    0x00
};


/******************************************************************************
Numbers
*******************************************************************************/

static const uint8_t FONT_0[8] = {
    0x0E,
    0x11,
    0x13,
    0x15,
    0x19,
    0x11,
    0x11,
    0x0E
};

static const uint8_t FONT_1[8] = {
    0x06,
    0x0E,
    0x16,
    0x06,
    0x06,
    0x06,
    0x06,
    0x1F
};

static const uint8_t FONT_2[8] = {
    0x0E,
    0x11,
    0x01,
    0x06,
    0x08,
    0x10,
    0x11,
    0x1F
};

static const uint8_t FONT_3[8] = {
    0x0E,
    0x11,
    0x01,
    0x06,
    0x01,
    0x11,
    0x11,
    0x0E
};

static const uint8_t FONT_4[8] = {
    0x02,
    0x06,
    0x0A,
    0x12,
    0x1F,
    0x02,
    0x02,
    0x02
};

static const uint8_t FONT_5[8] = {
    0x1F,
    0x10,
    0x10,
    0x1E,
    0x01,
    0x11,
    0x11,
    0x0E
};

static const uint8_t FONT_6[8] = {
    0x0E,
    0x11,
    0x10,
    0x1E,
    0x11,
    0x11,
    0x11,
    0x0E
};

static const uint8_t FONT_7[8] = {
    0x1F,
    0x11,
    0x01,
    0x02,
    0x04,
    0x04,
    0x04,
    0x04
};

static const uint8_t FONT_8[8] = {
    0x0E,
    0x11,
    0x11,
    0x0E,
    0x11,
    0x11,
    0x11,
    0x0E
};

static const uint8_t FONT_9[8] = {
    0x0E,
    0x11,
    0x11,
    0x0F,
    0x01,
    0x01,
    0x11,
    0x0E
};

/******************************************************************************
Uppercase Alphabet
*******************************************************************************/

static const uint8_t FONT_UPPER_A[8] = {
    0x0E,
    0x11,
    0x11,
    0x1F,
    0x11,
    0x11,
    0x11,
    0x11
};

static const uint8_t FONT_UPPER_B[8] = {
    0x1E,
    0x11,
    0x11,
    0x1E,
    0x11,
    0x11,
    0x11,
    0x1E
};

static const uint8_t FONT_UPPER_C[8] = {
    0x0E,
    0x11,
    0x10,
    0x10,
    0x10,
    0x11,
    0x0E,
    0x0E
};

static const uint8_t FONT_UPPER_D[8] = {
    0x1E,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x1E
};

static const uint8_t FONT_UPPER_E[8] = {
    0x1F,
    0x10,
    0x10,
    0x1E,
    0x10,
    0x10,
    0x10,
    0x1F
};

static const uint8_t FONT_UPPER_F[8] = {
    0x1F,
    0x10,
    0x10,
    0x1E,
    0x10,
    0x10,
    0x10,
    0x10
};

static const uint8_t FONT_UPPER_G[8] = {
    0x0E,
    0x11,
    0x10,
    0x10,
    0x17,
    0x11,
    0x11,
    0x0E
};


static const uint8_t FONT_UPPER_H[8] = {
  0x11,
  0x11,
  0x11,
  0x1F,
  0x1F,
  0x11,
  0x11,
  0x11
};

static const uint8_t FONT_UPPER_I[8] = {
    0x1F,
    0x0A,
    0x0A,
    0x0A,
    0x0A,
    0x0A,
    0x0A,
    0x1F
};

static const uint8_t FONT_UPPER_J[8] = {
    0x0E,
    0x05,
    0x05,
    0x05,
    0x05,
    0x15,
    0x15,
    0x0E
};

static const uint8_t FONT_UPPER_K[8] = {
    0x12,
    0x14,
    0x18,
    0x1C,
    0x1C,
    0x14,
    0x12,
    0x11
};

static const uint8_t FONT_UPPER_L[8] = {
    0x10,
    0x10,
    0x10,
    0x10,
    0x10,
    0x10,
    0x1F,
    0x1F
};

static const uint8_t FONT_UPPER_M[8] = {
    0x11,
    0x1B,
    0x1B,
    0x15,
    0x15,
    0x11,
    0x11,
    0x11
};

static const uint8_t FONT_UPPER_N[8] = {
    0x11,
    0x19,
    0x19,
    0x1D,
    0x15,
    0x13,
    0x11,
    0x11
};

static const uint8_t FONT_UPPER_O[8] = {
    0x0E,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x0E
};

static const uint8_t FONT_UPPER_P[8] = {
    0x1E,
    0x11,
    0x11,
    0x1E,
    0x10,
    0x10,
    0x10,
    0x10
};

static const uint8_t FONT_UPPER_Q[8] = {
    0x0E,
    0x11,
    0x11,
    0x11,
    0x15,
    0x19,
    0x16,
    0x0D
};

static const uint8_t FONT_UPPER_R[8] = {
    0x1E,
    0x11,
    0x11,
    0x1F,
    0x18,
    0x14,
    0x12,
    0x11
};

static const uint8_t FONT_UPPER_S[8] = {
    0x0E,
    0x11,
    0x11,
    0x0E,
    0x01,
    0x01,
    0x11,
    0x0E
};

static const uint8_t FONT_UPPER_T[8] = {
    0x1F,
    0x04,
    0x04,
    0x04,
    0x04,
    0x04,
    0x04,
    0x04
};

static const uint8_t FONT_UPPER_U[8] = {
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x0E
};

static const uint8_t FONT_UPPER_V[8] = {
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x0A,
    0x0A,
    0x04
};

static const uint8_t FONT_UPPER_W[8] = {
    0x11,
    0x11,
    0x11,
    0x15,
    0x15,
    0x1B,
    0x11,
    0x11
};

static const uint8_t FONT_UPPER_X[8] = {
    0x11,
    0x11,
    0x0A,
    0x0A,
    0x04,
    0x0A,
    0x0A,
    0x11
};

static const uint8_t FONT_UPPER_Y[8] = {
    0x11,
    0x11,
    0x11,
    0x0A,
    0x04,
    0x04,
    0x04,
    0x04
};

static const uint8_t FONT_UPPER_Z[8] = {
    0x1F,
    0x01,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x1F
};

/******************************************************************************
Lowercase Alphabet
*******************************************************************************/

static const uint8_t FONT_LOWER_A[8] = {
    0x00,
    0x00,
    0x0E,
    0x01,
    0x0F,
    0x11,
    0x0F,
    0x00
};

static const uint8_t FONT_LOWER_B[8] = {
    0x10,
    0x10,
    0x1E,
    0x11,
    0x11,
    0x11,
    0x1E,
    0x00
};

static const uint8_t FONT_LOWER_C[8] = {
    0x00,
    0x00,
    0x0E,
    0x10,
    0x10,
    0x11,
    0x0E,
    0x00
};

static const uint8_t FONT_LOWER_D[8] = {
    0x01,
    0x01,
    0x0F,
    0x11,
    0x11,
    0x11,
    0x0F,
    0x00
};

static const uint8_t FONT_LOWER_E[8] = {
    0x00,
    0x00,
    0x0E,
    0x11,
    0x1F,
    0x10,
    0x0E,
    0x00
};

static const uint8_t FONT_LOWER_F[8] = {
    0x06,
    0x09,
    0x08,
    0x1C,
    0x08,
    0x08,
    0x08,
    0x00
};

static const uint8_t FONT_LOWER_G[8] = {
    0x00,
    0x00,
    0x0F,
    0x11,
    0x11,
    0x0F,
    0x01,
    0x0E
};

static const uint8_t FONT_LOWER_H[8] = {
    0x10,
    0x10,
    0x1E,
    0x11,
    0x11,
    0x11,
    0x11,
    0x00
};

static const uint8_t FONT_LOWER_I[8] = {
    0x04,
    0x00,
    0x0C,
    0x04,
    0x04,
    0x04,
    0x0E,
    0x00
};

static const uint8_t FONT_LOWER_J[8] = {
    0x02,
    0x00,
    0x06,
    0x02,
    0x02,
    0x12,
    0x12,
    0x0C
};

static const uint8_t FONT_LOWER_K[8] = {
    0x10,
    0x10,
    0x12,
    0x14,
    0x18,
    0x14,
    0x12,
    0x00
};

static const uint8_t FONT_LOWER_L[8] = {
    0x0C,
    0x04,
    0x04,
    0x04,
    0x04,
    0x04,
    0x0E,
    0x00
};

static const uint8_t FONT_LOWER_M[8] = {
    0x00,
    0x00,
    0x1A,
    0x15,
    0x15,
    0x11,
    0x11,
    0x00
};

static const uint8_t FONT_LOWER_N[8] = {
    0x00,
    0x00,
    0x1E,
    0x11,
    0x11,
    0x11,
    0x11,
    0x00
};

static const uint8_t FONT_LOWER_O[8] = {
    0x00,
    0x00,
    0x0E,
    0x11,
    0x11,
    0x11,
    0x0E,
    0x00
};

static const uint8_t FONT_LOWER_P[8] = {
    0x00,
    0x00,
    0x1E,
    0x11,
    0x11,
    0x1E,
    0x10,
    0x10
};

static const uint8_t FONT_LOWER_Q[8] = {
    0x00,
    0x00,
    0x0F,
    0x11,
    0x11,
    0x0F,
    0x01,
    0x01
};

static const uint8_t FONT_LOWER_R[8] = {
    0x00,
    0x00,
    0x1A,
    0x15,
    0x10,
    0x10,
    0x10,
    0x00
};

static const uint8_t FONT_LOWER_S[8] = {
    0x00,
    0x00,
    0x0E,
    0x10,
    0x0E,
    0x01,
    0x0E,
    0x00
};

static const uint8_t FONT_LOWER_T[8] = {
    0x04,
    0x04,
    0x0E,
    0x04,
    0x04,
    0x04,
    0x02,
    0x00
};

static const uint8_t FONT_LOWER_U[8] = {
    0x00,
    0x00,
    0x11,
    0x11,
    0x11,
    0x11,
    0x0F,
    0x00
};

static const uint8_t FONT_LOWER_V[8] = {
    0x00,
    0x00,
    0x11,
    0x11,
    0x11,
    0x0A,
    0x04,
    0x00
};

static const uint8_t FONT_LOWER_W[8] = {
    0x00,
    0x00,
    0x11,
    0x11,
    0x15,
    0x15,
    0x0A,
    0x00
};

static const uint8_t FONT_LOWER_X[8] = {
    0x00,
    0x00,
    0x11,
    0x0A,
    0x04,
    0x0A,
    0x11,
    0x00
};

static const uint8_t FONT_LOWER_Y[8] = {
    0x00,
    0x00,
    0x11,
    0x11,
    0x0F,
    0x01,
    0x0E,
    0x00
};

static const uint8_t FONT_LOWER_Z[8] = {
    0x00,
    0x00,
    0x1F,
    0x02,
    0x04,
    0x08,
    0x1F,
    0x00
};

/******************************************************************************
Font Array
*******************************************************************************/

extern const uint8_t * const FONT_ARRAY[128] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    FONT_SPACE,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    FONT_PERIOD,
    0,
    FONT_0,
    FONT_1,
    FONT_2,
    FONT_3,
    FONT_4,
    FONT_5,
    FONT_6,
    FONT_7,
    FONT_8,
    FONT_9,
    FONT_COLON,
    0,
    0,
    0,
    0,
    0,
    0,
    FONT_UPPER_A,
    FONT_UPPER_B,
    FONT_UPPER_C,
    FONT_UPPER_D,
    FONT_UPPER_E,
    FONT_UPPER_F,
    FONT_UPPER_G,
    FONT_UPPER_H,
    FONT_UPPER_I,
    FONT_UPPER_J,
    FONT_UPPER_K,
    FONT_UPPER_L,
    FONT_UPPER_M,
    FONT_UPPER_N,
    FONT_UPPER_O,
    FONT_UPPER_P,
    FONT_UPPER_Q,
    FONT_UPPER_R,
    FONT_UPPER_S,
    FONT_UPPER_T,
    FONT_UPPER_U,
    FONT_UPPER_V,
    FONT_UPPER_W,
    FONT_UPPER_X,
    FONT_UPPER_Y,
    FONT_UPPER_Z,
    0,
    0,
    0,
    0,
    0,
    0,
    FONT_LOWER_A,
    FONT_LOWER_B,
    FONT_LOWER_C,
    FONT_LOWER_D,
    FONT_LOWER_E,
    FONT_LOWER_F,
    FONT_LOWER_G,
    FONT_LOWER_H,
    FONT_LOWER_I,
    FONT_LOWER_J,
    FONT_LOWER_K,
    FONT_LOWER_L,
    FONT_LOWER_M,
    FONT_LOWER_N,
    FONT_LOWER_O,
    FONT_LOWER_P,
    FONT_LOWER_Q,
    FONT_LOWER_R,
    FONT_LOWER_S,
    FONT_LOWER_T,
    FONT_LOWER_U,
    FONT_LOWER_V,
    FONT_LOWER_W,
    FONT_LOWER_X,
    FONT_LOWER_Y,
    FONT_LOWER_Z,
    0,
    0,
    0,
    0,
    0
};
