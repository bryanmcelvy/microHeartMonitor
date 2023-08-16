import numpy as np

# ADC
m = 2**11 / 3.3
b = 2**11

with open("python/adc_lookup.txt", "w") as file:
    file.write("static const float32_t ADC_LOOKUP[4096] = {\n" + (4 * " "))

    for y in range(4096):
        x = np.float32((y - b) / m)

        file.write(f"{x}")
        if y == 4095:  # last number
            file.write("\n};")
        elif (y + 1) % 4 == 0:
            file.write(",\n" + (4 * " "))
        else:
            file.write(", ")

# Sine Wave
# TODO: Generate sine wave lookup table
