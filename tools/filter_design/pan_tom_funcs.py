import numpy as np
from scipy import signal

''' 
–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
Pre-Processing
–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
** According to Martinek, P wave occurs up to 5 [Hz] and T wave occurs up to 7 [Hz]; try 10-20 [Hz] band for QRS
'''
def apply_lpf(xn):
    '''
    This function applies the PT algorithm's low pass filter to input signal `xn`
    '''
    # b = np.zeros(13)
    # b[0] = 1
    # b[6] = -2
    # b[12] = 1

    # a = np.zeros(len(b))
    # a[:3] = [1, -2, 1]
    
    b, a = signal.iirfilter(2, 20/100, btype='lowpass', ftype='butter')
    
    yn = signal.sosfiltfilt(signal.tf2sos(b,a), xn)
    
    # yn = signal.filtfilt(b, a, xn)
    
    return yn

def apply_hpf(xn):
    '''
    This function applies the PT algorithm's high pass filter to input signal `xn`
    '''
    # b = np.zeros(33)
    # b[0] = -1/32
    # b[16] = 1
    # b[17] = -1
    # b[32] = 1/32

    # a = [1, -1]
    
    b, a = signal.iirfilter(2, 12/100, btype='highpass', ftype='butter')
    
    yn = signal.sosfiltfilt(signal.tf2sos(b, a), xn)
    
    return yn

def apply_derfilt(xn):
    '''
    This function applies the PT algorithm's derivative filter to input signal `xn`
    '''
    
    fs = 200; f_nyq = fs / 2
    
    h = np.array( [1, 2, 0, -2, -1] ) / 8
    yn = signal.lfilter(h, 1, xn)
    
    return yn

def square_and_integrate(xn):
    '''
    This function squares and applies a 30-point moving average to input signal `xn`
    '''
    
    xn = np.square(xn)
    
    M = 10
    h = np.ones(M) / M
    yn = signal.filtfilt(h, 1, xn)
    
    return yn

'''
–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
Decision Rules
–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
'''
# Fiducial Marking
def find_fid_marks(xn):
    '''
    This function marks local peaks in input signal `xn` as potential QRS candidates.
    The fiducial marks must be spaced apart by at least 200 [ms] (40 samples @ fs = 200 [Hz] ); 
    if a peak is found within this range, the one with the largest amplitude is taken to be the 
    correct peak and the other is ignored.
    '''
    
    fid_marks = [0]     # Array of fiducial mark indices
    n_prev_mark = 0     # Index of previous fiducial mark
    count_since_prev = 1           # Number of samples since previous peak
    
    for n in range(1, len(xn)-1):
        
        # Verify that point at index `n` is a peak
        if ( xn[n] > xn[n-1] ) and ( xn[n] > xn[n+1] ):
            
            # Verify that peak appears ≥40 samples (200 [ms]) after previous mark
            if count_since_prev < 40:
                
                # Replace the last value in `fid_marks` with whichever index has the largest amplitude
                if xn[n] > xn[n_prev_mark]:
                    fid_marks[-1] = n
                    n_prev_mark = n
                    count_since_prev = 0
                
                else:
                    count_since_prev += 1
            
            else:
                fid_marks.append(n)
                n_prev_mark = n
                count_since_prev = 0
        
        else:
            count_since_prev += 1
    
    fid_marks = np.array(fid_marks[1:])
    
    return fid_marks

# Adaptive Thresholding
def init_levels(xn):
    '''
    This function initalizes the values for the signal level and noise level.
    '''
    
    # Only use first 2 seconds of data
    N = 400
    xn = xn[:N] if (len(xn) > N) else xn
    
    # Calc. signal level and noise level
    sig_level = 0.25 * np.max(xn)
    noise_level = 0.5 * np.mean(xn)
    
    return (sig_level, noise_level)

def update_level(peak_amp, level):
    '''
    This function updates the inputted signal level or noise level.
    '''
    
    level = (0.125 * peak_amp) + (0.875 * level)
    
    return level

def update_threshold(sig_level, noise_level):
    '''
    This function calculates a new threshold using the inputted signal and noise levels.
    '''
    
    threshold = noise_level + ( 0.25 * (sig_level - noise_level) )
    
    return threshold

'''
–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
Multi-Functions
–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
'''

def preprocess_ecg(xn):
    '''
    This function preprocesses input signal `xn` and returns a filtered, squared, and integrated output signal `yn`
    '''
    yn = apply_lpf(xn)
    yn = apply_hpf(yn)
    yn = apply_derfilt(yn)
    yn = square_and_integrate(yn)
    
    return yn

def apply_thresholding(xn):
    '''
    This function applies the adaptive thresholding procedure to input signal `xn`.
    '''
    
    fid_marks = find_fid_marks(xn)
    
    N_marks = len(fid_marks)
    sig_levs = np.zeros(N_marks)
    noise_levs = np.zeros(N_marks)
    thresh_vals = np.zeros(N_marks)
    
    # Initialize signal/noise levels and threshold
    sig_lev_current, noise_lev_current = init_levels(xn)
    thresh_val_current = update_threshold(sig_lev_current, noise_lev_current)
    
    # Apply Thresholding
    for i, mark in enumerate(fid_marks):
        
        if xn[mark] > thresh_val_current:
            sig_lev_current = update_level(xn[mark], sig_lev_current)
        else:
            noise_lev_current = update_level(xn[mark], noise_lev_current)
            fid_marks[i] = -1
        
        thresh_val_current = update_threshold(sig_lev_current, noise_lev_current)
        
        sig_levs[i] = sig_lev_current
        noise_levs[i] = noise_lev_current
        thresh_vals[i] = thresh_val_current
    
    new_mark_idx = np.not_equal(fid_marks, -1)
    
    qrs = fid_marks[new_mark_idx]
    sig_levs = sig_levs[new_mark_idx]
    noise_levs = noise_levs[new_mark_idx]
    thresh_vals = thresh_vals[new_mark_idx]
        
    return (qrs, (sig_levs, noise_levs, thresh_vals))

def detect_QRS(xn):
    yn = preprocess_ecg(xn)
    qrs, _ = apply_thresholding(yn)
    
    return qrs
