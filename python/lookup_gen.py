import numpy as np

# ADC
m = 2**11 / 3.3
b = 2**11

with open("python/adc_lookup.txt", "w") as file:
    file.write("static const double ADC_LOOKUP[4096] = {\n\t")

    for y in range(4096):
        x = np.float16((y - b) / m)
        file.write(f"{x}")

        if (y + 1) % 5 == 0:  # multiple of 5
            file.write(",\n\t")
        elif y == 4095:  # last number
            file.write("\n};")
        else:
            file.write(", ")

# Sine Wave
# TODO: Generate sine wave lookup table
