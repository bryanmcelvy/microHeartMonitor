import numpy as np

# ADC
m = 2**11 / 3.3
b = 2**11

lookup_str = "{"
for y in range(4096):
    x = np.single( (y-b)/m )
    lookup_str = (lookup_str + f"{x}, ") if (y < 4095) else (lookup_str + f"{x}}};")
    
with open("python/adc_lookup.txt", 'w') as file:
    file.write(lookup_str)
    
# Sine Wave
#TODO: Generate sine wave lookup table
