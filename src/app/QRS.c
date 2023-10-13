/**
 * @addtogroup qrs
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for QRS detection module.
 *
 *          The algorithm used in this file is a simplified version of the Pan-Tompkins algorithm.
 *          Specifically, this version currently only uses the integrated signal for the
 *          thresholding, and also completely omits the searchback and T wave discrimination parts
 *          of the original.
 */

#include "QRS.h"

/*******************************************************************************
SECTIONS
        Preprocessor Directives
        Static Declarations
        Digital Filters
        Main Functions
        Static Function Definitions
********************************************************************************/

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "dsp/statistics_functions.h"

#include <stdbool.h>
#include <stdint.h>

#define QRS_NUM_FID_MARKS               40

#define FLOAT_COMPARE_TOLERANCE         (float32_t)(1E-5f)
#define IS_GREATER(X, Y)                (bool) ((X - Y) > FLOAT_COMPARE_TOLERANCE)
#define IS_PEAK(X_MINUS_1, X, X_PLUS_1) (bool) (IS_GREATER(X, X_MINUS_1) && IS_GREATER(X, X_PLUS_1))

/*******************************************************************************
Static Declarations
********************************************************************************/

/**
 * @brief                       Mark local peaks in the input signal `y` as potential
 *                              candidates for QRS complexes (AKA "fiducial marks").
 *
 * @param[in] yn                Array containing the preprocessed ECG signal \f$ y[n] \f$
 * @param[in] fidMarkArray      Array to place the fiducial mark's sample indices into.
 * @param[out] numMarks         Number of identified fiducial marks
 *
 * @post                        `fidMarkArray` will hold the values of the fiducial marks.
 */
static uint8_t QRS_findFiducialMarks(const float32_t yn[], uint16_t fidMarkArray[]);

/**
 * @brief                       Initialize the signal and noise levels for the QRS detector
 *                              using the initial block of input signal data.
 *
 * @param[in] yn                Array containing the preprocessed ECG signal \f$ y[n] \f$
 * @param[in] sigLvlPtr         Pointer to variable holding the signal level value.
 * @param[in] noiseLvlPtr       Pointer to variable holding the noise level value.
 *
 * @post                        The signal and noise levels are initialized.
 */
static void QRS_initLevels(const float32_t yn[], float32_t * sigLvlPtr, float32_t * noiseLvlPtr);

/**
 * @brief                       Update the signal level (if a fiducial mark is a confirmed peak)
 *                              or the noise level (if a fiducial mark is rejected).
 *
 * @param[in] peakAmplitude     Amplitude of the fiducial mark in signal \f$ y[n] \f$
 * @param[in] level             The current value of the signal level or noise level
 * @param[out] newLevel         The updated value of the signal level or noise level
 */
static float32_t QRS_updateLevel(const float32_t peakAmplitude, float32_t level);

/**
 * @brief                       Update the amplitude threshold used to identify peaks based on the
 *                              signal and noise levels.
 *
 * @param[in] signalLevel       Current signal level.
 * @param[in] noiseLevel        Current noise level.
 * @param[out] threshold        New threshold to use for next comparison.
 *
 * @see                         QRS_updateLevel(), QRS_applyDecisionRules
 */
static float32_t QRS_updateThreshold(const float32_t signalLevel, const float32_t noiseLevel);

static struct {
    bool isCalibrated;

    float32_t signalLevel;                                  ///< estimated signal level
    float32_t noiseLevel;                                   ///< estimated noise level
    float32_t threshold;                                    ///< amplitude threshold

    uint16_t fidMarkArray[QRS_NUM_FID_MARKS];               /// array to hold fidMark indices
    float32_t utilityBuffer1[QRS_NUM_FID_MARKS];
    float32_t utilityBuffer2[QRS_NUM_FID_MARKS];
} Detector = { false, 0.0f, 0.0f, 0.0f, { 0 }, { 0 }, { 0 } };

/*******************************************************************************
Digital Filters
********************************************************************************/
/** @name Digital Filters */               /// @{

enum {
    // Bandpass Filter
    NUM_STAGES_BANDPASS = 4,
    NUM_COEFF_HIGHPASS = NUM_STAGES_BANDPASS * 5,
    STATE_BUFF_SIZE_BANDPASS = NUM_STAGES_BANDPASS * 4,

    // Derivative Filter
    NUM_COEFF_DERFILT = 5,
    BLOCK_SIZE_DERFILT = 1,
    STATE_BUFF_SIZE_DERFILT = NUM_COEFF_DERFILT + BLOCK_SIZE_DERFILT - 1,

    // Moving Average Filter
    BLOCK_SIZE_MOVAVG = 1,
    NUM_COEFF_MOVAVG = 10,
    STATE_BUFF_SIZE_MOVAVG = NUM_COEFF_MOVAVG + BLOCK_SIZE_MOVAVG - 1,
};

/* Filter Coefficients */

// clang-format off
static const float32_t COEFF_BANDPASS[NUM_COEFF_HIGHPASS] = {
    // Section 1
    0.002937758108600974f, 0.005875516217201948f, 0.002937758108600974f, 
    1.0485996007919312f, -0.2961403429508209f, 
    // Section 2
    1.0f, 2.0f, 1.0f, 
    1.3876197338104248f, -0.492422878742218f, 
    // Section 3
    1.0f, -2.0f, 1.0f, 
    1.3209134340286255f, -0.6327387690544128f, 
    // Section 4
    1.0f, -2.0f, 1.0f, 
    1.6299355030059814f, -0.7530401945114136f, 
};

static const float32_t COEFF_DERFILT[NUM_COEFF_DERFILT] = {
    -0.125f, -0.25f, 0.0f, 0.25f, 0.125f
};

static const float32_t COEFF_MOVAVG[NUM_COEFF_MOVAVG] = {
    0.10000000149011612f, 0.10000000149011612f, 0.10000000149011612f, 
    0.10000000149011612f, 0.10000000149011612f, 0.10000000149011612f,
    0.10000000149011612f, 0.10000000149011612f, 0.10000000149011612f, 0.10000000149011612f
};

typedef arm_biquad_casd_df1_inst_f32 IIR_Filt_t;
typedef arm_fir_instance_f32 FIR_Filt_t;

static float32_t stateBuffer_bandPass[STATE_BUFF_SIZE_BANDPASS] = { 0 };
static const IIR_Filt_t bandpassFiltStruct = { NUM_STAGES_BANDPASS, stateBuffer_bandPass, COEFF_BANDPASS };
static const IIR_Filt_t * const bandpassFilter = &bandpassFiltStruct;

static float32_t stateBuffer_DerFilt[STATE_BUFF_SIZE_DERFILT] = { 0 };
static const FIR_Filt_t derivativeFiltStruct = { NUM_COEFF_DERFILT, stateBuffer_DerFilt, COEFF_DERFILT };
static const FIR_Filt_t * const derivativeFilter = &derivativeFiltStruct;

static float32_t stateBuffer_MovingAvg[STATE_BUFF_SIZE_MOVAVG] = { 0 };
static const FIR_Filt_t movingAvgFiltStruct = { NUM_COEFF_MOVAVG, stateBuffer_MovingAvg, COEFF_MOVAVG };
static const FIR_Filt_t * const movingAverageFilter = &movingAvgFiltStruct;

// clang-format on

/** @} */               // Digital Filters

/*******************************************************************************
Main Functions
********************************************************************************/

/** @name Interface Functions */               /// @{

void QRS_Init(void) {
    /**
     * This function originally initialized the filter `struct`s but now does nothing since those
     * have been made `const` and their initialization functions have been removed entirely.
     */
    return;
}

void QRS_Preprocess(const float32_t xn[], float32_t yn[]) {
    /**
     * This function uses the same overall preprocessing pipeline as the original Pan-Tompkins
     * algorithm, but the high-pass and low-pass filters have been replaced with ones generated
     * using Scipy.
     *
     * @image latex software/qrs_preproc.png
     * @image latex software/qrs_preproc_output.png "Output of each preprocessing step."
     */

    // copy samples from input buffer `xn` to output buffer `yn`
    if(((uint32_t) &xn[0]) != ((uint32_t) &yn[0])) {               // skip if they're the same
        for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
            yn[n] = xn[n];
        }
    }

    arm_biquad_cascade_df1_f32(bandpassFilter, yn, yn, QRS_NUM_SAMP);

    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        // NOTE: filtering one-at-a-time so that the state buffer doesn't take up too much memory
        arm_fir_f32(derivativeFilter, &yn[n], &yn[n], BLOCK_SIZE_DERFILT);
    }

    // square
    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        yn[n] = yn[n] * yn[n];
    }

    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        // NOTE: filtering one-at-a-time so that the state buffer doesn't take up too much memory
        arm_fir_f32(movingAverageFilter, &yn[n], &yn[n], BLOCK_SIZE_MOVAVG);
    }

    return;
}

float32_t QRS_applyDecisionRules(const float32_t yn[]) {
    // TODO: Write implementation explanation

    // copy variables from `Detector` for readability
    float32_t signalLevel = Detector.signalLevel;
    float32_t noiseLevel = Detector.noiseLevel;
    float32_t threshold = Detector.threshold;

    uint16_t * fidMarkArray = Detector.fidMarkArray;

    float32_t * timeBuffer = Detector.utilityBuffer1;                    // time in [s] of each peak
    float32_t * heartRateBuffer = Detector.utilityBuffer2;               // HR in [BPM]

    // calibrate detector on first pass
    if(Detector.isCalibrated == false) {
        QRS_initLevels(yn, &signalLevel, &noiseLevel);
        threshold = QRS_updateThreshold(signalLevel, noiseLevel);
        Detector.isCalibrated = true;
    }

    // classify fiducial marks as signal (confirmed R peaks) or noise
    uint8_t numMarks = QRS_findFiducialMarks(yn, fidMarkArray);
    uint8_t numPeaks = 0;

    for(uint8_t idx = 0; idx < numMarks; idx++) {
        uint16_t n = fidMarkArray[idx];

        if(IS_GREATER(yn[n], threshold)) {
            timeBuffer[numPeaks] = n * QRS_SAMP_PERIOD_SEC;
            numPeaks += 1;

            signalLevel = QRS_updateLevel(yn[n], signalLevel);
        }
        else {
            noiseLevel = QRS_updateLevel(yn[n], noiseLevel);
        }

        threshold = QRS_updateThreshold(signalLevel, noiseLevel);
    }

    // store updated values in `Detector`
    Detector.signalLevel = signalLevel;
    Detector.noiseLevel = noiseLevel;
    Detector.threshold = threshold;

    // calculate RR interval and convert to HR
    for(uint8_t idx = 0; idx < (numPeaks - 1); idx++) {
        heartRateBuffer[idx] = 60.0f / (timeBuffer[idx + 1] - timeBuffer[idx]);
    }

    float32_t avgHeartRate_bpm;
    arm_mean_f32(heartRateBuffer, numPeaks, &avgHeartRate_bpm);

    return avgHeartRate_bpm;
}

float32_t QRS_runDetection(const float32_t xn[], float32_t yn[]) {
    QRS_Preprocess(xn, yn);
    float32_t heartRate_bpm = QRS_applyDecisionRules(yn);

    return heartRate_bpm;
}

/** @} */               // Interface Functions

/*******************************************************************************
Static Function Definitions
********************************************************************************/

/** @name Implementation-specific Functions */               /// @{

static void QRS_initLevels(const float32_t yn[], float32_t * sigLvlPtr, float32_t * noiseLvlPtr) {
    float32_t max;
    uint32_t maxIdx;
    arm_max_f32(yn, QRS_SAMP_FREQ * 2, &max, &maxIdx);
    *sigLvlPtr = 0.25f * max;

    float32_t mean;
    arm_mean_f32(yn, QRS_SAMP_FREQ * 2, &mean);
    *noiseLvlPtr = 0.5f * mean;

    return;
}

static uint8_t QRS_findFiducialMarks(const float32_t yn[], uint16_t fidMarkArray[]) {
    uint8_t numMarks = 0;                                    // running counter of peak candidates
    uint16_t countSincePrev = 1;               // samples checked since previous peak candidate
    uint16_t n_prevMark = 0;                   // sample number of previous peak candidate

    // reset fidMarkArray
    for(uint8_t i = 0; i < QRS_NUM_FID_MARKS; i++) {
        fidMarkArray[i] = 0;
    }

    for(uint16_t n = 1; n < (QRS_NUM_SAMP - 1); n++) {
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

static float32_t QRS_updateLevel(const float32_t peakAmplitude, float32_t level) {
    /**
     * \f$
     * signalLevel_1    = f(peakAmplitude, signalLevel_0)
     *                  = \frac{1}{8}peakAmplitude + \frac{7}{8}signalLevel_0
     * \f$
     *
     * \f$
     * noiseLevel_1     = f(peakAmplitude, noiseLevel_0)
     *                  = \frac{1}{8}peakAmplitude + \frac{7}{8}noiseLevel_0
     * \f$
     */
    return ((0.125f * peakAmplitude) + (0.875f * level));
}

static float32_t QRS_updateThreshold(const float32_t signalLevel, const float32_t noiseLevel) {
    /**
     * \f$
     * threshold = f(signalLevel, noiseLevel) = noiseLevel + 0.25(signalLevel - noiseLevel)
     * \f$
     */
    return (noiseLevel + (0.25f * (signalLevel - noiseLevel)));
}

/** @} */               // Implementation-specific Functions

/** @} */               // qrs
