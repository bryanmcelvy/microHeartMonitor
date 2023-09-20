from jds6600 import SigGen
import numpy as np
import pandas as pd
import time

# Load waveform
ecg = pd.read_csv("tools/JDS6600/ecg_waveform.csv").iloc[:, 1]
ecg = ecg.values

# Initialize signal generator
sig_gen = SigGen("/dev/tty.usbserial-14240")
sig_gen.turnChannel_OFF(3)
time.sleep(1)

# Write waveform to signal generator
sig_gen.createArbitraryWaveform(1, ecg)

print("Done!")
