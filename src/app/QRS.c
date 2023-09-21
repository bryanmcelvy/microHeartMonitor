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

/* NOLINTBEGIN(bugprone-narrowing-conversions) */

// IIR Filters
typedef arm_biquad_casd_df1_inst_f32 IIR_Filt_t;

static IIR_Filt_t LowPassFilt = { 0 };
static const float32_t COEFF_LOWPASS[5] = { 0.06744, 0.1349, 0.06744, -1.143, 0.4128 };
static float32_t stateBuffer_LowPass[4];

static IIR_Filt_t HighPassFilt = { 0 };
static const float32_t COEFF_HIGHPASS[5] = { 0.7656, -1.531, 0.7656, -1.476, 0.587 };
static float32_t stateBuffer_HighPass[4];

// FIR Filters
typedef arm_fir_instance_f32 FIR_Filt_t;

static FIR_Filt_t DerFilt = { 0 };
static const float32_t COEFF_DERFILT[5] = { -0.125, -0.25, 0, 0.25, 0.125 };
static float32_t stateBuffer_DerFilt[5 + QRS_NUM_SAMP - 1];

static FIR_Filt_t MovingAvgFilt = { 0 };
static const float32_t COEFF_MOVAVG[15] = { 0.06666667, 0.06666667, 0.06666667, 0.06666667,
                                            0.06666667, 0.06666667, 0.06666667, 0.06666667,
                                            0.06666667, 0.06666667, 0.06666667, 0.06666667,
                                            0.06666667, 0.06666667, 0.06666667 };
static float32_t stateBuffer_MovingAvg[15 + QRS_NUM_SAMP - 1];

/* NOLINTEND(bugprone-narrowing-conversions) */

/*******************************************************************************
Main Functions
********************************************************************************/

void QRS_Init(void) {

    // Initialize filters
    arm_biquad_cascade_df1_init_f32(&LowPassFilt, 1, COEFF_LOWPASS, stateBuffer_LowPass);
    arm_biquad_cascade_df1_init_f32(&HighPassFilt, 1, COEFF_HIGHPASS, stateBuffer_HighPass);
    arm_fir_init_f32(&DerFilt, 5, COEFF_DERFILT, stateBuffer_DerFilt, QRS_NUM_SAMP);
    arm_fir_init_f32(&MovingAvgFilt, 15, COEFF_MOVAVG, stateBuffer_MovingAvg, QRS_NUM_SAMP);

    return;
}

void QRS_Preprocess(float32_t inputBuffer[], float32_t outputBuffer[]) {
    // TODO: Write implementation explanation

    // high-pass filter
    arm_biquad_cascade_df1_f32(&HighPassFilt, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    // low-pass filter
    arm_biquad_cascade_df1_f32(&LowPassFilt, outputBuffer, inputBuffer, QRS_NUM_SAMP);

    // derivative filter
    arm_fir_f32(&DerFilt, inputBuffer, outputBuffer, QRS_NUM_SAMP);

    // square
    for(uint16_t n = 0; n < QRS_NUM_SAMP; n++) {
        inputBuffer[n] = outputBuffer[n] * outputBuffer[n];
    }

    // moving-average filter (i.e. integrate)
    arm_fir_f32(&MovingAvgFilt, inputBuffer, outputBuffer, QRS_NUM_SAMP);

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

inline float32_t QRS_RunDetection(float32_t inputBuffer[],
                                  float32_t outputBuffer[]);               // defined in header

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
