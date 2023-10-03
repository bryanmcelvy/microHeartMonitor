/**
 * @addtogroup qrs
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for QRS module.
 */

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#include "QRS.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "dsp/statistics_functions.h"

#include <stdbool.h>
#include <stdint.h>

#define QRS_NUM_FID_MARKS 13

/*******************************************************************************
Static Function Declarations
********************************************************************************/

static uint8_t QRS_findFiducialMarks(float32_t preprocessedData[], uint16_t fidMarkArray[]);
static void QRS_initLevels(float32_t preprocessedData[]);
static float32_t QRS_updateLevel(float32_t peakAmplitude, float32_t level);
static float32_t QRS_UpdateThreshold(void);

/*******************************************************************************
Static Variables
********************************************************************************/

static struct {
    bool isCalibrated;

    float32_t signalLevel;
    float32_t noiseLevel;
    float32_t threshold;

    uint16_t fidMarkArray[QRS_NUM_FID_MARKS];
} Detector;

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
static IIR_Filt_t * const highPassFilter = &highPassFiltStruct;

static float32_t stateBuffer_LowPass[STATE_BUFF_SIZE_LOWPASS] = { 0 };
static const IIR_Filt_t lowPassFiltStruct = { NUM_STAGES_LOWPASS, stateBuffer_LowPass, COEFF_LOWPASS };
static IIR_Filt_t * const lowPassFilter = &lowPassFiltStruct;

/* FIR Filters */
typedef arm_fir_instance_f32 FIR_Filt_t;

static float32_t stateBuffer_DerFilt[STATE_BUFF_SIZE_DERFILT] = { 0 };
static const FIR_Filt_t derivativeFiltStruct = { NUM_COEFF_DERFILT, stateBuffer_DerFilt, COEFF_DERFILT };
static FIR_Filt_t * const derivativeFilter = &derivativeFiltStruct;

static float32_t stateBuffer_MovingAvg[STATE_BUFF_SIZE_MOVAVG] = { 0 };
static const FIR_Filt_t movingAvgFiltStruct = { NUM_COEFF_MOVAVG, stateBuffer_MovingAvg, COEFF_MOVAVG };
static FIR_Filt_t * const movingAverageFilter = &movingAvgFiltStruct;

// clang-format on

/*******************************************************************************
Main Functions
********************************************************************************/

void QRS_Init(void) {
    return;
}

void QRS_Preprocess(float32_t inputBuffer[], float32_t outputBuffer[]) {
    // TODO: Write implementation explanation

    // high-pass filter
    arm_biquad_cascade_df1_f32(highPassFilter, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    // low-pass filter
    for(uint16_t idx = 0; idx < QRS_NUM_SAMP; idx++) {
        inputBuffer[idx] = outputBuffer[idx];
    }
    arm_biquad_cascade_df1_f32(lowPassFilter, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    // derivative filter
    for(uint16_t idx = 0; idx < QRS_NUM_SAMP; idx++) {
        inputBuffer[idx] = outputBuffer[idx];
    }
    arm_fir_f32(derivativeFilter, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    // square
    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        inputBuffer[n] = inputBuffer[n] * inputBuffer[n];
    }

    // moving-average filter (i.e. integrate)
    arm_fir_f32(movingAverageFilter, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    return;
}

float32_t QRS_ApplyDecisionRules(float32_t inputBuffer[]) {
    // TODO: Write implementation explanation

    // calibrate detector on first pass
    if(Detector.isCalibrated == false) {
        QRS_initLevels(inputBuffer);
        Detector.threshold = QRS_UpdateThreshold();
        Detector.isCalibrated = true;
    }

    // classify points and update levels/threshold as needed
    uint8_t numMarks = QRS_findFiducialMarks(inputBuffer, Detector.fidMarkArray);

    uint16_t sumPeakIdx = 0;
    uint16_t numPeaks = 0;
    for(uint16_t idx = 0; idx < numMarks; idx++) {
        uint16_t n = Detector.fidMarkArray[idx];
        if(inputBuffer[n] > Detector.threshold) {
            Detector.signalLevel = QRS_updateLevel(inputBuffer[n], Detector.signalLevel);
            sumPeakIdx += n;
            numPeaks += 1;
        }
        else {
            Detector.noiseLevel = QRS_updateLevel(inputBuffer[n], Detector.noiseLevel);
        }

        Detector.threshold = QRS_UpdateThreshold();
    }

    // take average and convert to [bpm]
    float32_t avgHeartRate_bpm = (numPeaks / (float32_t) sumPeakIdx) * QRS_SAMP_FREQ * 60;

    return avgHeartRate_bpm;
}

float32_t QRS_RunDetection(float32_t inputBuffer[], float32_t outputBuffer[]) {
    QRS_Preprocess(inputBuffer, outputBuffer);
    float32_t heartRate_bpm = QRS_ApplyDecisionRules(outputBuffer);

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
static uint8_t QRS_findFiducialMarks(float32_t yn[], uint16_t fidMarkArray[]) {
    uint8_t numMarks = 0;                      // running counter of peak candidates
    uint16_t countSincePrev = 1;               // samples checked since previous peak candidate
    uint16_t n_prevMark = 0;                   // sample number of previous peak candidate

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
                // Replace the previous mark with the index with the largest amplitude
                if(preprocData[n] > preprocData[n_prevMark]) {
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
 * @brief                       Initialize the signal and noise levels for the QRS detector.
 *
 * @param[in] yn                Array containing the preprocessed ECG signal \f$ y[n] \f$
 */
static void QRS_initLevels(float32_t yn[]) {
    float32_t mean;
    float32_t max;
    uint32_t maxIdx;

    arm_max_f32(yn, QRS_NUM_SAMP, &max, &maxIdx);
    Detector.signalLevel = 0.25f * max;

    arm_mean_f32(yn, QRS_NUM_SAMP, &mean);
    Detector.noiseLevel = 0.5f * mean;

    return;
}

/**
 * @brief                       Update signal or noise level based on a confirmed peak's amplitude.
 *
 * @param[in] peakAmplitude     Amplitude of the peak in signal \f$y[n]\f$
 * @param[in] level             The current value of the signal level or noise level
 * @param[out] float32_t        The updated value of the signal level or noise level
 */
static float32_t QRS_updateLevel(float32_t peakAmplitude, float32_t level) {
    return ((0.125f * peakAmplitude) + (0.875f * level));
}

/**
 * @brief                       Update the amplitude threshold used to identify peaks based on the
 *                              signal and noise levels.
 *
 *                              \f$ threshold = f(signalLevel, noiseLevel) \f$
 *
 * @param[out] float32_t        New threshold
 */
static float32_t QRS_UpdateThreshold(void) {
    return (Detector.noiseLevel + (0.25f * (Detector.signalLevel - Detector.noiseLevel)));
}

/** @} */               // qrs
