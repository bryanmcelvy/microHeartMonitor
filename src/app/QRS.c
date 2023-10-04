/**
 * @addtogroup qrs
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for QRS module.
 */

/*******************************************************************************
SECTIONS
        Static Declarations
        Digital Filters
        Main Functions
        Static Function Definitions
********************************************************************************/

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#include "QRS.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "dsp/statistics_functions.h"

#include <stdbool.h>
#include <stdint.h>

#define QRS_NUM_FID_MARKS               13

#define FLOAT_COMPARE_TOLERANCE         (float32_t)(1E-5f)
#define IS_GREATER(X, Y)                (bool) ((X - Y) > FLOAT_COMPARE_TOLERANCE)
#define IS_LESSER(X, Y)                 (bool) ((Y - X) > FLOAT_COMPARE_TOLERANCE)
#define IS_PEAK(X_MINUS_1, X, X_PLUS_1) (bool) (IS_GREATER(X, X_MINUS_1) && IS_GREATER(X, X_PLUS_1))

/*******************************************************************************
Static Declarations
********************************************************************************/

static uint8_t QRS_findFiducialMarks(float32_t yn[], int16_t fidMarkArray[]);
static void QRS_initLevels(const float32_t yn[]);
static float32_t QRS_updateLevel(float32_t peakAmplitude, float32_t level);
static float32_t QRS_updateThreshold(void);

static struct {
    bool isCalibrated;

    float32_t signalLevel;
    float32_t noiseLevel;
    float32_t threshold;

    int16_t fidMarkArray[QRS_NUM_FID_MARKS];               /// array to hold fidMark indices
    float32_t HR_Array[QRS_NUM_FID_MARKS];                 /// array to hold RR intervals
    float32_t buffer[QRS_NUM_SAMP];                        /// utility buffer
} Detector = { false, 0.0f, 0.0f, 0.0f, { 0 }, { 0 }, { 0 } };

/*******************************************************************************
Digital Filters
********************************************************************************/

enum {
    // High Pass Filter
    NUM_STAGES_HIGHPASS = 2,
    NUM_COEFF_HIGHPASS = NUM_STAGES_HIGHPASS * 5,
    STATE_BUFF_SIZE_HIGHPASS = NUM_STAGES_HIGHPASS * 4,

    // Low Pass Filter
    NUM_STAGES_LOWPASS = 2,
    NUM_COEFF_LOWPASS = NUM_STAGES_LOWPASS * 5,
    STATE_BUFF_SIZE_LOWPASS = NUM_STAGES_LOWPASS * 4,

    // Derivative Filter
    NUM_COEFF_DERFILT = 5,
    STATE_BUFF_SIZE_DERFILT = NUM_COEFF_DERFILT + QRS_NUM_SAMP - 1,

    // Moving Average Filter
    NUM_COEFF_MOVAVG = 30,
    STATE_BUFF_SIZE_MOVAVG = NUM_COEFF_MOVAVG + QRS_NUM_SAMP - 1,
};

/* Filter Coefficients */

// clang-format off
static const float32_t COEFF_HIGHPASS[NUM_COEFF_HIGHPASS] = {
    // Section 1
    0.6089446544647217f, -1.2178893089294434f, 0.6089446544647217f, 
    1.3876197338104248f, -0.492422878742218f, 
    // Section 2
    1.0f, -2.0f, 1.0f, 
    1.6299355030059814f, -0.7530401945114136f, 
};

static const float32_t COEFF_LOWPASS[NUM_COEFF_LOWPASS] = {
    // Section 1
    0.004824343137443066f, 0.009648686274886131f, 0.004824343137443066f, 
    1.0485996007919312f, -0.2961403429508209f, 
    // Section 2
    1.0f, 2.0f, 1.0f, 
    1.3209134340286255f, -0.6327387690544128f, 
};

static const float32_t COEFF_DERFILT[NUM_COEFF_DERFILT] = { -0.125f, -0.25f, 0.0f, 0.25f, 0.125f };

static const float32_t COEFF_MOVAVG[NUM_COEFF_MOVAVG] = {
    0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f,
    0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f,
    0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f,
    0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f,
    0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f,
    0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f,
    0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f, 0.03333333507180214f,
    0.03333333507180214f, 0.03333333507180214f
};

/* IIR Filters */
typedef arm_biquad_casd_df1_inst_f32 IIR_Filt_t;

static float32_t stateBuffer_HighPass[STATE_BUFF_SIZE_HIGHPASS] = { 0 };
static const IIR_Filt_t highPassFiltStruct = { NUM_STAGES_HIGHPASS, stateBuffer_HighPass, COEFF_HIGHPASS };
static const IIR_Filt_t * const highPassFilter = &highPassFiltStruct;

static float32_t stateBuffer_LowPass[STATE_BUFF_SIZE_LOWPASS] = { 0 };
static const IIR_Filt_t lowPassFiltStruct = { NUM_STAGES_LOWPASS, stateBuffer_LowPass, COEFF_LOWPASS };
static const IIR_Filt_t * const lowPassFilter = &lowPassFiltStruct;

/* FIR Filters */
typedef arm_fir_instance_f32 FIR_Filt_t;

static float32_t stateBuffer_DerFilt[STATE_BUFF_SIZE_DERFILT] = { 0 };
static const FIR_Filt_t derivativeFiltStruct = { NUM_COEFF_DERFILT, stateBuffer_DerFilt, COEFF_DERFILT };
static const FIR_Filt_t * const derivativeFilter = &derivativeFiltStruct;

static float32_t stateBuffer_MovingAvg[STATE_BUFF_SIZE_MOVAVG] = { 0 };
static const FIR_Filt_t movingAvgFiltStruct = { NUM_COEFF_MOVAVG, stateBuffer_MovingAvg, COEFF_MOVAVG };
static const FIR_Filt_t * const movingAverageFilter = &movingAvgFiltStruct;

// clang-format on

/*******************************************************************************
Main Functions
********************************************************************************/

void QRS_Init(void) {
    return;
}

void QRS_Preprocess(const float32_t xn[], float32_t yn[]) {
    /**
     * This function uses the same overall preprocessing pipeline as the original Pan-Tompkins
     * algorithm, but the high-pass and low-pass filters have been replaced with ones generated
     * using Scipy.
     */

    // copy samples from `xn` to the detector's utility buffer
    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        Detector.buffer[n] = xn[n];
    }

    // high-pass filter
    arm_biquad_cascade_df1_f32(highPassFilter, Detector.buffer, yn, QRS_NUM_SAMP);

    // low-pass filter
    for(uint16_t idx = 0; idx < QRS_NUM_SAMP; idx++) {
        Detector.buffer[idx] = yn[idx];
    }
    arm_biquad_cascade_df1_f32(lowPassFilter, Detector.buffer, yn, QRS_NUM_SAMP);

    // derivative filter
    for(uint16_t idx = 0; idx < QRS_NUM_SAMP; idx++) {
        Detector.buffer[idx] = yn[idx];
    }
    arm_fir_f32(derivativeFilter, Detector.buffer, yn, QRS_NUM_SAMP);

    // square
    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        Detector.buffer[n] = Detector.buffer[n] * Detector.buffer[n];
    }

    // moving-average filter (i.e. integrate)
    arm_fir_f32(movingAverageFilter, Detector.buffer, yn, QRS_NUM_SAMP);

    return;
}

float32_t QRS_applyDecisionRules(const float32_t yn[]) {
    // TODO: Write implementation explanation

    // calibrate detector on first pass
    if(Detector.isCalibrated == false) {
        QRS_initLevels(yn);
        Detector.threshold = QRS_updateThreshold();
        Detector.isCalibrated = true;
    }

    // classify points and update levels/threshold as needed
    uint8_t numMarks = QRS_findFiducialMarks(yn, Detector.fidMarkArray);

    uint16_t sumPeakIdx = 0;
    uint16_t numPeaks = 0;
    for(uint16_t idx = 0; idx < numMarks; idx++) {
        uint16_t n = Detector.fidMarkArray[idx];
        if(IS_GREATER(yn[n], Detector.threshold)) {
            Detector.signalLevel = QRS_updateLevel(yn[n], Detector.signalLevel);
            sumPeakIdx += n;
            numPeaks += 1;
        }
        else {
            Detector.noiseLevel = QRS_updateLevel(yn[n], Detector.noiseLevel);
        }

        Detector.threshold = QRS_UpdateThreshold();
    }

    // take average and convert to [bpm]
    float32_t avgHeartRate_bpm = (numPeaks / (float32_t) sumPeakIdx) * QRS_SAMP_FREQ * 60;

    return avgHeartRate_bpm;
}

float32_t QRS_runDetection(const float32_t xn[], float32_t yn[]) {
    QRS_Preprocess(xn, yn);
    float32_t heartRate_bpm = QRS_applyDecisionRules(yn);

    return heartRate_bpm;
}

/*******************************************************************************
Static Function Definitions
********************************************************************************/

/**
 * @brief                   Mark local peaks in the input signal `y` as potential
 *                          candidates for QRS complexes (AKA "fiducial marks").
 *
 * @param[in] yn            Array containing the preprocessed ECG signal \f$ y[n] \f$
 * @param[in] fidMarkArray  Array to place the fiducial mark's sample indices into.
 * @param[out] uint8_t      Number of identified fiducial marks
 */
static uint8_t QRS_findFiducialMarks(float32_t yn[], int16_t fidMarkArray[]) {
    uint8_t numMarks = 0;                      // running counter of peak candidates
    uint16_t countSincePrev = 1;               // samples checked since previous peak candidate
    uint16_t n_prevMark = 0;                   // sample number of previous peak candidate

    for(uint8_t i = 0; i < QRS_NUM_FID_MARKS; i++) {
        fidMarkArray[i] = 0;
    }

    for(int16_t n = 1; n < (QRS_NUM_SAMP - 1); n++) {
        if(IS_PEAK(yn[n - 1], yn[n], yn[n + 1])) {               // Verify `y[n]` is a peak
            /**
             * The fiducial marks must be spaced apart by at least 200 [ms] (40 samples
             * @ fs = 200 [Hz]). If a peak is found within this range, the one with the
             * largest amplitude is taken to be the correct peak and the other is ignored.
             */
            if(countSincePrev >= 40) {
                fidMarkArray[numMarks] = n;
                numMarks += 1;

                n_prevMark = n;
                countSincePrev = 0;
            }
            else if(countSincePrev < 40) {
                if(IS_GREATER(yn[n], yn[n_prevMark])) {
                    fidMarkArray[numMarks - 1] = n;
                    n_prevMark = n;
                    countSincePrev = 0;
                }
                else {
                    countSincePrev += 1;
                }
            }
        }
        else {
            countSincePrev += 1;
        }
    }

    return numMarks;
}

/**
 * @brief                       Initialize the signal and noise levels for the QRS detector
 *                              using the initial block of input signal data.
 *
 * @param[in] yn                Array containing the preprocessed ECG signal \f$ y[n] \f$
 *
 * @post                        The detector's signal and noise levels are initialized.
 */
static void QRS_initLevels(const float32_t yn[]) {
    float32_t max;
    uint32_t maxIdx;
    arm_max_f32(yn, QRS_NUM_SAMP, &max, &maxIdx);
    Detector.signalLevel = 0.25f * max;

    float32_t mean;
    arm_mean_f32(yn, QRS_NUM_SAMP, &mean);
    Detector.noiseLevel = 0.5f * mean;

    return;
}

/**
 * @brief                       Update signal or noise level based on a confirmed peak's amplitude.
 *
 * @param[in] peakAmplitude     Amplitude of the peak in signal \f$y[n]\f$
 * @param[in] level             The current value of the signal level or noise level
 * @param[out] newLevel         The updated value of the signal level or noise level
 */
static float32_t QRS_updateLevel(float32_t peakAmplitude, float32_t level) {
    return ((0.125f * peakAmplitude) + (0.875f * level));
}

/**
 * @brief                       Update the amplitude threshold used to identify peaks based on the
 *                              signal and noise levels.
 *
 * @param[out] threshold        New threshold to use for next comparison.
 */
static float32_t QRS_updateThreshold(void) {
    /**
     * \f$
     * threshold = f(signalLevel, noiseLevel) = noiseLevel + 0.25(signalLevel - noiseLevel)
     * \f$
     */
    return (Detector.noiseLevel + (0.25f * (Detector.signalLevel - Detector.noiseLevel)));
}

/** @} */               // qrs
