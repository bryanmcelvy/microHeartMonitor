import cmsisdsp as cmsis
import numpy as np

V_OUT_MIN = 0.
V_OUT_MAX = 3.3
NUM_VALS = (1 << 12)

v_out = np.linspace(V_OUT_MIN, V_OUT_MAX, NUM_VALS, endpoint=False, dtype=np.float32)
v_out = cmsis.arm_float_to_q31(v_out / V_OUT_MAX)

with open("tools/lookup_table/adc_lookup_q31.txt", "w") as file:
    file.write("static const q31_t ADC_LOOKUP[4096] = {\n" + (4 * " "))
    for n in range(NUM_VALS):
        file.write(f"{v_out[n]}")
        if n == (NUM_VALS - 1):
            file.write("\n};")
        elif (n + 1) % 6 == 0:
            file.write(",\n" + (4 * " "))
        else:
            file.write(", ")
