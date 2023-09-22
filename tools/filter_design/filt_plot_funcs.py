import matplotlib.pyplot as plt
from matplotlib.patches import Circle

import numpy as np
from scipy import signal

''' Time Domain Plot Functions '''

def plot_impulse_resp(b, a, ax=None):
    
    # Calculate the impulse response
    hn = signal.lfilter(b, a, signal.unit_impulse(len(b)))
    
    # Pad 10 zeros on each side
    N1 = len(hn) - (len(hn) % 10) + 11
    
    pad_len = (10, N1-len(hn))
    hn = np.pad(hn, pad_len, constant_values=[0])
    N = len(hn)
    n = np.arange(0, N) - pad_len[0]
    
    # Plot
    if not ax:
        plt.figure()
    else:
        plt.sca(ax)
        
    plt.grid()
    
    plt.plot(n, hn, 'sk', markersize=3.5)
    plt.plot(n, hn, '-k', linewidth=0.5)
    
    plt.xlim([np.min(n), np.max(n)])
    plt.ylim(
                np.add( 
                        np.mean(hn), 
                        np.multiply( np.max(hn)-np.min(hn),
                                [-1, 1]
                                )
                        )
                )
    plt.xticks(np.arange(np.min(n), np.max(n)+5, 10))
    
    plt.title("Impulse Response")
    plt.xlabel("Sample number")
    plt.ylabel("Amplitude")

def plot_step_resp(b, a, ax=None):
    
    # Calculate the impulse response
    hn = signal.lfilter(b, a, signal.unit_impulse(len(b)))
    
    # Pad 10 zeros on each side
    N1 = len(hn) - (len(hn) % 10) + 11
    
    pad_len = (10, N1-len(hn))
    hn = np.pad(hn, pad_len, constant_values=[0])
    N = len(hn)
    n = np.arange(0, N) - pad_len[0]
    
    # Calculate step response
    hn = np.cumsum(hn)
    
    # Plot
    if not ax:
        plt.figure()
    else:
        plt.sca(ax)
        
    plt.grid()
    
    plt.plot(n, hn, 'sk', markersize=3.5)
    plt.plot(n, hn, '-k', linewidth=0.5)
    
    plt.xlim([np.min(n), np.max(n)])
    plt.ylim(
                np.add( 
                        np.mean(hn), 
                        np.multiply( np.max(hn)-np.min(hn),
                                [-1, 1]
                                )
                        )
                )
    plt.xticks(np.arange(np.min(n), np.max(n)+5, 10))
    
    plt.title("Step Response")
    plt.xlabel("Sample number")
    plt.ylabel("Amplitude")

''' Frequency Domain Plot Functions '''

def plot_freq_resp(b, a, N=512, fs=1, in_dB = False, ax=None):
    
    freq, H = signal.freqz(b, a, N, fs=fs)
    H = 20 * np.log10(np.abs(H)) if (in_dB) else np.abs(H)
    
    if not ax:
        plt.figure()
    else:
        plt.sca(ax)
        
    plt.grid()
    
    plt.plot(freq, H, '-k')
    
    plt.xlim([freq[0], freq[-1]])
    plt.xticks(np.linspace(0.0, 0.5, 6) * fs)
    
    plt.title("Frequency Response")
    if fs == 1: plt.xlabel("Normalized Frequency [Hz]")
    else: plt.xlabel("Frequency [Hz]")
    if in_dB: plt.ylabel("Magnitude [dB]")
    else: plt.ylabel("Magnitude")

def plot_group_delay(b, a, N=512, fs=1, ax=None):
    
    freq, grd = signal.group_delay([b, a], N, fs=fs)
    grd = [np.ceil(np.max(grd)) if ( (np.max(grd) - np.min(grd)) < 2 ) else val for val in grd]
    
    
    if not ax:
        plt.figure()
    else:
        plt.sca(ax)
        
    plt.grid()
    
    plt.plot(freq, grd, '-k')
    
    plt.xlim([freq[0], freq[-1]])
    plt.xticks(np.linspace(0.0, 0.5, 6) * fs)
    
    plt.title("Group Delay")
    if fs == 1: plt.xlabel("Normalized Frequency [Hz]")
    else: plt.xlabel("Frequency [Hz]")
    plt.ylabel("Delay [samples]")

def plot_pole_zero(b, a, ax=None, filt_name=None):
    
    [z, p, _] = signal.tf2zpk(b, a)
    
    # Create plot
    if not ax:
        plt.figure()
        _, ax = plt.subplots(figsize=np.multiply(5, [1, 1]))
    else:
        plt.sca(ax)
        
    plt.grid()
    plt.xlim([-1.1, 1.1]); plt.xticks(np.linspace(-1, 1, 5))
    plt.ylim([-1.1, 1.1]); plt.yticks(np.linspace(-1, 1, 11))

    plot_title = "Pole-Zero Plot" if (filt_name == None) else filt_name
    plt.title(plot_title)
    plt.xlabel("Real Part")
    plt.ylabel("Imaginary Part")
    
    # Add circle
    pole_zero_plot = plt.Circle((0,0), 1, facecolor='w', edgecolor='k')
    ax.add_patch(pole_zero_plot)

    # Add poles and zeros
    plt.plot(z.real, z.imag, 'ob', markersize=5)
    plt.plot(p.real, p.imag, 'xr', markersize=7.5)

''' Multi-Plot Functions '''

def plot_domain_t(b, a, ax1=None, ax2=None, filt_name=None):
    
    if not (ax1 and ax2):
        _, [ax1, ax2] = plt.subplots(1, 2, figsize=[9, 4], tight_layout=True)
    
    if filt_name:
        plt.suptitle(filt_name)
        
    plot_impulse_resp(b, a, ax1)
    plot_step_resp(b, a, ax2)  

def plot_domain_f(b, a, fs=1, in_dB=False, ax1=None, ax2=None, filt_name=None):
    
    if not (ax1 and ax2):
        _, [ax1, ax2] = plt.subplots(1, 2, figsize=[9, 4], tight_layout=True)
    
    if filt_name:
        plt.suptitle(filt_name)
    
    plot_freq_resp(b, a, fs=fs, in_dB=in_dB, ax=ax1)
    plot_group_delay(b, a, fs=fs, ax=ax2)  
