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

static uint8_t QRS_FindFiducialMarks(float32_t preprocessedData[], int16_t fidMarkArray[]);
static void QRS_InitLevels(float32_t preprocessedData[]);
static float32_t QRS_UpdateLevel(float32_t peakAmplitude, float32_t level);
static float32_t QRS_UpdateThreshold(void);

/*******************************************************************************
Static Variables
********************************************************************************/

static struct {
    bool isCalibrated;

    float32_t signalLevel;
    float32_t noiseLevel;
    float32_t threshold;

    int16_t fidMarkArray[QRS_NUM_FID_MARKS];
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

/* IIR Filters */
typedef arm_biquad_casd_df1_inst_f32 IIR_Filt_t;

static IIR_Filt_t highPassFiltStruct = { 0 };
static IIR_Filt_t * const highPassFilter = &highPassFiltStruct;
static float32_t stateBuffer_HighPass[STATE_BUFF_SIZE_HIGHPASS];

static IIR_Filt_t lowPassFiltStruct = { 0 };
static IIR_Filt_t * const lowPassFilter = &lowPassFiltStruct;
static float32_t stateBuffer_LowPass[STATE_BUFF_SIZE_LOWPASS];

/* FIR Filters */
typedef arm_fir_instance_f32 FIR_Filt_t;

static FIR_Filt_t derivativeFiltStruct = { 0 };
static FIR_Filt_t * const derivativeFilter = &derivativeFiltStruct;
static float32_t stateBuffer_DerFilt[STATE_BUFF_SIZE_DERFILT];

static FIR_Filt_t movingAvgFiltStruct = { 0 };
static FIR_Filt_t * const movingAverageFilter = &movingAvgFiltStruct;
static float32_t stateBuffer_MovingAvg[STATE_BUFF_SIZE_MOVAVG];

// clang-format off
static const float32_t COEFF_HIGHPASS[NUM_COEFF_HIGHPASS] = {
    // Section 1
    0.6089446544647217f, -1.2178893089294434f, 0.6089446544647217f, 
    1.3876197338104248f, -0.492422878742218f, 
    // Section 2
    1.0f, -2.0f, 1.0f, 
    1.6299355030059814f, -0.7530401945114136f, 
};

/* Filter Coefficients */
static const float32_t COEFF_LOWPASS[NUM_COEFF_LOWPASS] = {
    // Section 1
    0.004824343137443066f, 0.009648686274886131f, 0.004824343137443066f, 
    1.0485996007919312f, -0.2961403429508209f, 
    // Section 2
    1.0f, 2.0f, 1.0f, 
    1.3209134340286255f, -0.6327387690544128f, 
};
// clang-format on

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

/*******************************************************************************
Main Functions
********************************************************************************/

void QRS_Init(void) {

    // Initialize filters
    arm_biquad_cascade_df1_init_f32(highPassFilter, NUM_COEFF_HIGHPASS, COEFF_HIGHPASS,
                                    stateBuffer_HighPass);
    arm_biquad_cascade_df1_init_f32(lowPassFilter, NUM_STAGES_LOWPASS, COEFF_LOWPASS,
                                    stateBuffer_LowPass);
    arm_fir_init_f32(derivativeFilter, NUM_COEFF_DERFILT, COEFF_DERFILT, stateBuffer_DerFilt,
                     QRS_NUM_SAMP);
    arm_fir_init_f32(movingAverageFilter, NUM_COEFF_MOVAVG, COEFF_MOVAVG, stateBuffer_MovingAvg,
                     QRS_NUM_SAMP);

    return;
}

void QRS_Preprocess(float32_t inputBuffer[], float32_t outputBuffer[]) {
    // TODO: Write implementation explanation

    // high-pass filter
    arm_biquad_cascade_df1_f32(highPassFilter, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    // low-pass filter
    arm_biquad_cascade_df1_f32(lowPassFilter, outputBuffer, inputBuffer, QRS_NUM_SAMP);

    // derivative filter
    arm_fir_f32(derivativeFilter, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    // square
    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        inputBuffer[n] = outputBuffer[n] * outputBuffer[n];
    }

    // moving-average filter (i.e. integrate)
    arm_fir_f32(movingAverageFilter, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    return;
}

float32_t QRS_ApplyDecisionRules(float32_t inputBuffer[]) {
    // TODO: Write implementation explanation

    // calibrate detector on first pass
    if(Detector.isCalibrated == false) {
        QRS_InitLevels(inputBuffer);
        Detector.threshold = QRS_UpdateThreshold();
        Detector.isCalibrated = true;
    }

    // classify points and update levels/threshold as needed
    uint8_t numMarks = QRS_FindFiducialMarks(inputBuffer, Detector.fidMarkArray);

    uint16_t sumPeakIdx = 0;
    uint16_t numPeaks = 0;
    for(uint16_t idx = 0; idx < numMarks; idx++) {
        int16_t n = Detector.fidMarkArray[idx];
        if(inputBuffer[n] > Detector.threshold) {
            Detector.signalLevel = QRS_UpdateLevel(inputBuffer[n], Detector.signalLevel);
            sumPeakIdx += n;
            numPeaks += 1;
        }
        else {
            Detector.noiseLevel = QRS_UpdateLevel(inputBuffer[n], Detector.noiseLevel);
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

static uint8_t QRS_FindFiducialMarks(float32_t preprocData[], int16_t fidMarkArray[]) {
    /**
     * This function marks local peaks in  the input signal as potential QRS
     * candidates. The fiducial marks must be spaced apart by at least 200 [ms]
     * (40 samples @ fs = 200 [Hz] ). If a peak is found within this range, the
     * one with the largest amplitude is taken to be the correct peak and the other
     * is ignored.
     */

    uint8_t numMarks = 0;
    uint16_t n_prevMark = 0;
    uint16_t countSincePrev = 1;

    for(uint8_t i = 0; i < QRS_NUM_FID_MARKS; i++) {
        fidMarkArray[i] = 0;
    }

    for(int16_t n = 1; n < (QRS_NUM_SAMP - 1); n++) {
        // Verify that sample [n] is a peak
        if((preprocData[n] > preprocData[n - 1]) && (preprocData[n] > preprocData[n + 1])) {
            // Verify that the peak appears ≥40 samples (200 [ms]) after previous mark
            if(countSincePrev >= 40) {
                fidMarkArray[numMarks++] = n;
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

static void QRS_InitLevels(float32_t preprocessedData[]) {
    float32_t mean, max;
    uint32_t max_idx;

    arm_max_f32(preprocessedData, QRS_NUM_SAMP, &max, &max_idx);
    Detector.signalLevel = 0.25f * max;

    arm_mean_f32(preprocessedData, QRS_NUM_SAMP, &mean);
    Detector.noiseLevel = 0.5f * mean;

    return;
}

static float32_t QRS_UpdateLevel(float32_t peakAmplitude, float32_t level) {
    return (float32_t) ((0.125 * peakAmplitude) + (0.875 * level));
}

static float32_t QRS_UpdateThreshold(void) {
    return (float32_t) (Detector.noiseLevel +
                        (0.25 * (Detector.signalLevel - Detector.noiseLevel)));
}

/** @} */               // qrs
