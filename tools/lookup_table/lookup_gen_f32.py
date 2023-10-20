#**************************************************************************************************
# File:         /tools/lookup_table/lookup_gen_f32.py
# Description:  Script for generating the DAQ's lookup table
#**************************************************************************************************

import numpy as np

# define constants
NUM_VALS = (1 << 12)
V_OUT_MAX = 3.3
V_OUT_MIN = 0
V_IN_MAX = 5.5
V_IN_MIN = -5.5

# calc. v_out
v_ADC = np.arange(NUM_VALS)                     # [0, 4095]
m_ADC = (NUM_VALS - 1) / V_OUT_MAX
v_out = np.float32(v_ADC / m_ADC)               # [0, 3.3)

# calc. v_in
m_1 = (V_OUT_MAX - V_OUT_MIN) / (V_IN_MAX - V_IN_MIN)
b_1 = 1.65
v_in = (v_out - b_1) / m_1                      # [-5.5, 5.5)


# output to .txt file
with open("tools/lookup_table/adc_lookup_f32.txt", "w") as file:
    file.write("static const float32_t DAQ_LOOKUP_TABLE[4096] = {\n" + (4 * " "))

    for n in range(NUM_VALS):
        file.write(f"{v_in[n]}")
        if n == (NUM_VALS - 1):
            file.write("\n};")
        elif (n + 1) % 4 == 0:
            file.write(",\n" + (4 * " "))
        else:
            file.write(", ")
