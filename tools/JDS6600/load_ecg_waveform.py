from jds6600 import SigGen
import numpy as np
import pandas as pd
import time

# Load waveform
ecg = np.array(pd.read_csv("tools/JDS6600/ecg_waveform.csv"))

# Initialize signal generator
sig_gen = SigGen("/dev/tty.usbserial-14230")
sig_gen.turnChannel_OFF(3)
time.sleep(2)

# Write waveform to signal generator
sig_gen.createArbitraryWaveform(1, ecg)
time.sleep(2)

print("Done!")
