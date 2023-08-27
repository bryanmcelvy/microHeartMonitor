# import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy import signal

# Select a single ECG waveform
n1 = 125
n2 = 325
data = pd.read_csv("data/101.csv")
ecg = data.iloc[n1:n2, 1]

# Resample to 2048 samples
N = 2048
samp = np.arange(N)
ecg = signal.resample(ecg, N)

# Normalize to a range of [0, 4095]
ecg = (ecg - np.min(ecg)) / (np.max(ecg) - np.min(ecg))
ecg *= 4095
ecg = np.floor(ecg).astype(int)

# plt.plot(samp, ecg)

# Save to .csv
ecg = pd.DataFrame(ecg)
ecg.to_csv("tools/JDS6600/ecg_waveform.csv", index=False, index_label=False)
