/**
 * @addtogroup qrs
 * @{
 *
 * @file
 * @author  Bryan McElvy
 * @brief   Source code for Detector module.
 */

/*******************************************************************************
Preprocessor Directives
********************************************************************************/

#include "QRS.h"

#include "FIFO.h"

#include "arm_math_types.h"
#include "dsp/filtering_functions.h"
#include "dsp/statistics_functions.h"

#include <math.h>
#include <string.h>
#include <sys/_stdint.h>

#define DETECTOR_NUM_SAMPLES 1000               // number of samples to process after calibration
#define SAMPLING_FREQUENCY   200

/*******************************************************************************
Static Variables
********************************************************************************/

static bool isCalibrated = false;
static float32_t signalLevel;
static float32_t noiseLevel;
static float32_t threshold;

static float32_t filtBuffer1[DETECTOR_NUM_SAMPLES];
static float32_t filtBuffer2[DETECTOR_NUM_SAMPLES];

static FIFO_t * Detector_Fifo;
static float32_t Detector_FifoBuffer[DETECTOR_NUM_SAMPLES];

/*******************************************************************************
Static Filter Variables
********************************************************************************/

// NOLINTBEGIN(bugprone-narrowing-conversions)
typedef arm_biquad_casd_df1_inst_f32 IIR_Filt_t;
typedef arm_fir_instance_f32 FIR_Filt_t;

static IIR_Filt_t LowPassFilt = { 0 };
static const float32_t COEFF_LOWPASS[5] = { 0.06744, 0.1349, 0.06744, -1.143, 0.4128 };
static float32_t stateBuffer_LowPass[4];

static IIR_Filt_t HighPassFilt = { 0 };
static const float32_t COEFF_HIGHPASS[5] = { 0.7656, -1.531, 0.7656, -1.476, 0.587 };
static float32_t stateBuffer_HighPass[4];

static FIR_Filt_t DerFilt = { 0 };
static const float32_t COEFF_DERFILT[5] = { -0.125, -0.25, 0, 0.25, 0.125 };
static float32_t stateBuffer_DerFilt[5 + DETECTOR_NUM_SAMPLES - 1];

static FIR_Filt_t MovingAvgFilt = { 0 };
static const float32_t COEFF_MOVAVG[15] = { 0.06666667, 0.06666667, 0.06666667, 0.06666667,
                                            0.06666667, 0.06666667, 0.06666667, 0.06666667,
                                            0.06666667, 0.06666667, 0.06666667, 0.06666667,
                                            0.06666667, 0.06666667, 0.06666667 };
static float32_t stateBuffer_MovingAvg[15 + DETECTOR_NUM_SAMPLES - 1];
// NOLINTEND(bugprone-narrowing-conversions)

/*******************************************************************************
Static Function Declarations
********************************************************************************/

// Preprocessing
static void Preprocess(void);

// Decision Rules
static void FindFiducialMarks(uint16_t * fidMarkArray, const uint16_t N);
static void InitLevels(void);
static float32_t UpdateLevel(float32_t peakAmplitude, float32_t level);
static float32_t UpdateThreshold(void);
static float32_t ApplyThresholding(void);

/*******************************************************************************
Main Functions
********************************************************************************/

void Detector_Init(void) {
    // Initialize filters
    arm_biquad_cascade_df1_init_f32(&LowPassFilt, 1, COEFF_LOWPASS, stateBuffer_LowPass);
    arm_biquad_cascade_df1_init_f32(&HighPassFilt, 1, COEFF_HIGHPASS, stateBuffer_HighPass);
    arm_fir_init_f32(&DerFilt, 5, COEFF_DERFILT, stateBuffer_DerFilt, DETECTOR_NUM_SAMPLES);
    arm_fir_init_f32(&MovingAvgFilt, 15, COEFF_MOVAVG, stateBuffer_MovingAvg, DETECTOR_NUM_SAMPLES);

    // Initialize internal FIFO buffer
    Detector_Fifo = FIFO_Init(Detector_FifoBuffer, DETECTOR_NUM_SAMPLES);

    return;
}

float32_t Detector_RunDetection(FIFO_t * inputDataFifo) {
    FIFO_TransferAll(inputDataFifo, Detector_Fifo);
    Preprocess();
    float32_t heartRate_bpm = ApplyThresholding();

    return heartRate_bpm;
}

/*******************************************************************************
Static Function Definitions
********************************************************************************/

static void Preprocess(void) {
    FIFO_Flush(Detector_Fifo, filtBuffer1);

    // high-pass filter
    arm_biquad_cascade_df1_f32(&HighPassFilt, filtBuffer1, filtBuffer2, DETECTOR_NUM_SAMPLES);

    // low-pass filter
    arm_biquad_cascade_df1_f32(&LowPassFilt, filtBuffer2, filtBuffer1, DETECTOR_NUM_SAMPLES);

    // derivative filter
    arm_fir_f32(&DerFilt, filtBuffer1, filtBuffer2, DETECTOR_NUM_SAMPLES);

    // square
    for(int n = 0; n < DETECTOR_NUM_SAMPLES; n++) {
        filtBuffer2[n] *= filtBuffer2[n];
    }

    // moving-average filter (i.e. integrate)
    arm_fir_f32(&MovingAvgFilt, filtBuffer2, filtBuffer1, DETECTOR_NUM_SAMPLES);

    // move back to FIFO
    for(int n = 0; n < DETECTOR_NUM_SAMPLES; n++) {
        FIFO_Put(Detector_Fifo, filtBuffer2[n]);
    }

    return;
}

/*******************************************************************************
Decision Rules
********************************************************************************/
static void FindFiducialMarks(uint16_t * fidMarkArray, const uint16_t N) {
    // TODO: Implement
    /**
     * This function marks local peaks in  the input signal as potential QRS
     * candidates. The fiducial marks must be spaced apart by at least 200 [ms]
     * (40 samples @ fs = 200 [Hz] ). If a peak is found within this range, the
     * one with the largest amplitude is taken to be the correct peak and the other
     * is ignored.
     */

    uint8_t currIdx = 0;
    uint16_t n_prevFidMark;
    uint16_t countSincePrev = 1;

    FIFO_PeekAll(Detector_Fifo, filtBuffer1);

    for(uint8_t i = 0; i < N; i++) {
        fidMarkArray[i] = 0;
    }

    for(int n = 1; n < (DETECTOR_NUM_SAMPLES - 1); n++) {
        // Verify that sample [n] is a peak
        if((filtBuffer1[n] > filtBuffer1[n - 1]) && (filtBuffer1[n] > filtBuffer1[n + 1])) {
            // Verify that peak appears ≥40 samples (200 [ms]) after previous mark
            if(countSincePrev < 40) {
                // Replace the previous mark with the index with the largest amplitude
                if(filtBuffer1[n] > filtBuffer1[n_prevFidMark]) {
                    fidMarkArray[currIdx - 1] = n;
                    n_prevFidMark = n;
                    countSincePrev = 0;
                }
                else {
                    countSincePrev += 1;
                }
            }
            else {
                fidMarkArray[currIdx++] = n;
                n_prevFidMark = n;
                countSincePrev = 0;
            }
        }
        else {
            countSincePrev += 1;
        }
    }
}

static void InitLevels(void) {
    float32_t mean, max;
    uint32_t max_idx;
    FIFO_PeekAll(Detector_Fifo, filtBuffer1);

    arm_max_f32(filtBuffer1, DETECTOR_NUM_SAMPLES, &max, &max_idx);
    signalLevel = 0.25f * max;

    arm_mean_f32(filtBuffer1, DETECTOR_NUM_SAMPLES, &mean);
    noiseLevel = 0.5f * mean;

    return;
}

static float32_t UpdateLevel(float32_t peakAmplitude, float32_t level) {
    return (float32_t) ((0.125 * peakAmplitude) + (0.875 * level));
}

static float32_t UpdateThreshold(void) {
    return (float32_t) (noiseLevel + (0.25 * (signalLevel - noiseLevel)));
}

static float32_t ApplyThresholding(void) {
    FIFO_Flush(Detector_Fifo, filtBuffer1);

    int16_t fidMarkArray[10];
    FindFiducialMarks(fidMarkArray, 10);

    if(isCalibrated == false) {
        InitLevels();
    }
    threshold = UpdateThreshold();

    for(uint16_t idx = 0; idx < 10; idx++) {
        int16_t n = fidMarkArray[idx];
        if(filtBuffer1[n] > threshold) {
            signalLevel = UpdateLevel(filtBuffer1[n], signalLevel);
        }
        else {
            noiseLevel = UpdateLevel(filtBuffer1[n], noiseLevel);
            fidMarkArray[idx] = -1;
        }

        threshold = UpdateThreshold();
    }

    uint16_t sumFidMarks = 0;
    uint16_t num_QRS = 0;
    for(uint16_t idx = 0; idx < 10; idx++) {
        if(fidMarkArray[idx] >= 0) {
            sumFidMarks += fidMarkArray[idx];
            num_QRS += 1;
        }
    }
    float32_t heartRate_samples = sumFidMarks / (float32_t) num_QRS;
    return heartRate_samples / (float32_t) SAMPLING_FREQUENCY;
}

/** @} */               // qrs
