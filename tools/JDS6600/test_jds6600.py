#**************************************************************************************************
# File:         /tools/JDS6600/test_jds6600.py
# Description:  Test script for JDS6600 library
#**************************************************************************************************

from jds6600 import SigGen
import time

sig_gen = SigGen("/dev/tty.usbserial-14130")

print("Turning on channel 1...\n")
sig_gen.turnChannel_ON(1)
time.sleep(2)

print("Turning off channel 1...\n")
sig_gen.turnChannel_OFF(1)
time.sleep(2)

print("Writing all 2s to arbitrary waveform number 2...\n")
vals = [2 for _ in range(2048)]
sig_gen.createArbitraryWaveform(2, vals)


print("...done.")
