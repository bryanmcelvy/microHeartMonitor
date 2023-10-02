import numpy as np
import pandas as pd
from scipy import signal
import wfdb.io

# fmt: off
patient_lst = [ 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 
                110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 
                120, 121, 122, 123, 124, 200, 201, 202, 203, 204, 
                205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 
                215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 
                225, 226, 227, 228, 229, 230, 231, 232, 233, 234]
# fmt: on

# Load data
for patient_num in patient_lst:
    file_name = f"./tools/data/mit-bih-arr/{patient_num}"

    try:
        data, _ = wfdb.io.rdsamp(file_name)
    except:
        print(f"Patient #{patient_num} not found.")
        continue

    data = np.transpose(data)[0]

    # Resample from 360 [Hz] to 200 [Hz]
    fs_old = 360
    fs_new = 200
    N = int((len(data) / fs_old) * fs_new)
    data = signal.resample(data, N)

    # Save to .csv
    data = pd.DataFrame(data)
    data.to_csv(f"./tools/data/csv/{patient_num}.csv")
