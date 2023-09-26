import numpy as np
import pandas as pd
from scipy import signal

# Select a single ECG waveform
data = pd.read_csv("data/101.csv")
ecg = data.iloc[500:675, 1]

# Resample to 2048 samples
ecg = signal.resample(ecg, 2048)

# Normalize to a range of [0, 4095]
ecg = (ecg - np.min(ecg)) / (np.max(ecg) - np.min(ecg))
ecg *= 4095
ecg = np.floor(ecg).astype(int)

# Save to .csv
ecg = pd.DataFrame(ecg)
ecg.to_csv("tools/JDS6600/ecg_waveform.csv")
