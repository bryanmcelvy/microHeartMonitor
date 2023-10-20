#**************************************************************************************************
# File:         tools/JDS6600/jds6600.py
# Description:  Library for communicating with a JDS6600 Signal Generator (or derivative) via serial port.
#**************************************************************************************************
import serial

class SigGen:
    """
    SECTIONS
        Initialization
        Channel Output Status
        Waveforms (Built-in)
        Frequency
        Amplitude
        Duty Cycle
        DC Offset
        Phase Offset
    """

    """Initialization"""

    def __init__(self, portName=None):  # "/dev/tty.usbserial-14130"
        # Create serial port object
        self.__serial_port = serial.Serial(
            baudrate=115200, bytesize=8, parity="N", stopbits=1
        )

        if portName is not None:
            self.__serial_port.port = portName

        # Attributes
        self.__output_status = [0, 0]
        self.__waveform = ["SINE", "SINE"]
        self.__freq = [0, 0]
        self.__amp = [0, 0]
        self.__duty_cycle = [0, 0]
        self.__DC_offset = [0, 0]
        self.__phase_offset = [0, 0]

        return

    def setSerialPort(self, portName):
        """!
        @brief              Set the name of the serial port to be connect to.

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] Vpp      Peak-to-peak voltage in [V].
        """

        self.__serial_port.port = portName

        return

    def __write(self, instruction_char, func_num, param_lst):
        # Format output string
        if func_num < 10:
            func_str = f"0{func_num}"
        else:
            func_str = str(func_num)

        output_str = f":{instruction_char}" + func_str + "="
        for param in param_lst:
            output_str += f"{param},"

        output_str += ".\r\n"

        # Send command and get response from the generator
        with self.__serial_port as ser:
            ser.write(bytes(output_str, "utf-8"))
            if instruction_char == "r" or instruction_char == "b":
                response = ser.readlines()
            else:
                response = ser.readline()

        print(response)

        return

    """Channel Output Status"""

    def getOutputStatus(self):
        """!
        @brief              Get the output status of the specified channel.
        """

        self.__write("w", 20, [])

        return

    def turnChannel_ON(self, ch_num=1):
        """!
        @brief              Turn a channel ON.

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        """

        if ch_num == 1:
            self.__output_status[0] = 1
        elif ch_num == 2:
            self.__output_status[1] = 1
        else:
            self.__output_status = [1, 1]

        self.__write("w", 20, self.__output_status)

        return

    def turnChannel_OFF(self, ch_num=1):
        """!
        @brief              Turn a channel OFF.

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        """

        if ch_num == 1:
            self.__output_status[0] = 0
        elif ch_num == 2:
            self.__output_status[1] = 0
        else:
            self.__output_status = [0, 0]

        self.__write("w", 20, self.__output_status)

        return

    """Waveforms (Built-in)"""
    waveform_dict = {
        "SINE": 0,
        "SQUARE": 1,
        "PULSE": 2,
        "TRIANGLE": 3,
        "SINE_PARTIAL": 4,
        "CMOS": 5,
        "DC_LEVEL": 6,
        "HALF_WAVE": 7,
        "FULL_WAVE": 8,
        "STEP_POS": 9,
        "STEP_NEG": 10,
        "NOISE": 11,
        "EXPONENTIAL": 12,
        "EXP_DECAY": 13,
        "MULTI_TONE": 14,
        "SINC": 15,
        "LORENZ": 16,
    }

    def setWaveform(self, ch_num=1, waveform="SINE"):
        """!
        @brief              Set the output waveform for a single channel from the built-in list (`waveform_list`).

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] waveform Name of built-in waveform to output from `waveform_list`.
        """

        if ch_num == 1:
            self.__waveform[0] = self.waveform_dict[waveform]
            self.__write("w", 21, self.__waveform[0])
        elif ch_num == 2:
            self.__waveform[1] = self.waveform_dict[waveform]
            self.__write("w", 22, self.__waveform[1])
        else:
            self.__waveform = [self.waveform_dict[waveform] for _ in range(2)]
            self.__write("w", 21, self.__waveform[0])
            self.__write("w", 22, self.__waveform[1])

        return

    """Frequency"""

    def setFreqHz(self, ch_num=1, f=0):
        """!
        @brief              Set the frequency in [Hz] for the specified channel.
                            Should be <= 15 [MHz].

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] f        Frequency in [Hz]. Should be <= 15E3 [Hz].
        """

        f *= 100

        if ch_num == 1:
            self.__freq[0] = f
            self.__write("w", 23, self.__freq[0])
        elif ch_num == 2:
            self.__freq[1] = f
            self.__write("w", 24, self.__freq[1])
        else:
            self.__freq = [f for _ in range(2)]
            self.__write("w", 23, self.__freq[0])
            self.__write("w", 24, self.__freq[1])

        return

    """Amplitude"""

    def setVpp_V(self, ch_num=1, Vpp=5):
        """!
        @brief              Set the chosen channel's peak-to-peak voltage \f$V_{pp}\f$ in [V].

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] Vpp      Peak-to-peak voltage in [V].
        """

        Vpp *= 1000

        self.setVpp_mV(ch_num, Vpp)

        return

    def setVpp_mV(self, ch_num=1, Vpp=5000):
        """!
        @brief              Set the chosen channel's peak-to-peak voltage \f$V_{pp}\f$ in [mV].

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] Vpp      Peak-to-peak voltage in [mV].
        """

        if ch_num == 1:
            self.__amp[0] = Vpp
            self.__write("w", 25, self.__freq[0])
        elif ch_num == 2:
            self.__amp[1] = Vpp
            self.__write("w", 26, self.__amp[1])
        else:
            self.__amp = [Vpp for _ in range(2)]
            self.__write("w", 25, self.__amp[0])
            self.__write("w", 26, self.__amp[1])

        return

    """Duty Cycle"""

    def setDutyCycle(self, ch_num=1, duty_cycle=50):
        """!
        @brief              Set the duty cycle of the chosen channel's waveform.
                            Only applicable when the waveform is "PULSE" or "TRIANGLE".

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] Vpp      Peak-to-peak voltage in [V].
        """

        duty_cycle = (duty_cycle % 101) * 10

        if ch_num == 1:
            self.__duty_cycle[0] = duty_cycle
            self.__write("w", 29, self.__freq[0])
        elif ch_num == 2:
            self.__duty_cycle[1] = duty_cycle
            self.__write("w", 30, self.__duty_cycle[1])
        else:
            self.__duty_cycle = [duty_cycle for _ in range(2)]
            self.__write("w", 29, self.__duty_cycle[0])
            self.__write("w", 30, self.__duty_cycle[1])

        return

    """DC Offset"""

    def setOffset_V(self, ch_num=1, offset_V=0):
        """!
        @brief              Set the chosen channel's offset voltage in [V].

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] Vpp      Offset voltage in [V].
        """

        offset_mV = offset_V * 1000
        self.setOffset_mV(ch_num, offset_mV)

        return

    def setOffset_mV(self, ch_num=1, offset_mV=0):
        """!
        @brief              Set the chosen channel's offset voltage in [mV].

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] Vpp      Offset voltage in [V].
        """

        if ch_num == 1:
            self.__DC_offset[0] = offset_mV
            self.__write("w", 27, self.__freq[0])
        elif ch_num == 2:
            self.__DC_offset[1] = offset_mV
            self.__write("w", 28, self.__DC_offset[1])
        else:
            self.__DC_offset = [offset_mV for _ in range(2)]
            self.__write("w", 27, self.__DC_offset[0])
            self.__write("w", 28, self.__DC_offset[1])

        return

    """Phase Offset"""

    def setPhase(self, phase_degrees=0):
        """!
        @brief              Set the channel 2's phase offset in [º].

        @param[in] Vpp      Phase offset in [º].
        """

        phase_degrees %= 361

        self.__phase_offset[1] = phase_degrees
        self.__write("w", 31, self.__phase_offset[1])

        return

    """ Arbitrary Waveforms """

    def setWaveformArbitrary(self, ch_num=1, waveform_num=1):
        """!
        @brief              Set the output waveform for a single channel to one of the 60 user-defined waveforms.

        @param[in] ch_num   `1` for channel 1, `2` for channel 2; any other number for both.
        @param[in] waveform Number (= [1, 60]) of arbitrary waveform to output.
        """

        # Ensure the waveform number is in the range
        waveform_num %= 60
        waveform_num = 1 if (waveform_num <= 0) else waveform_num

        if ch_num == 1:
            self.__waveform[0] = waveform_num + 100
            self.__write("w", 21, self.__waveform[0])
        elif ch_num == 2:
            self.__waveform[1] = waveform_num + 100
            self.__write("w", 22, self.__waveform[1])
        else:
            self.__waveform = [(waveform_num + 100) for _ in range(2)]
            self.__write("w", 21, self.__waveform[0])
            self.__write("w", 22, self.__waveform[1])

        return

    def createArbitraryWaveform(self, waveform_num, val_lst):
        if (0 < waveform_num <= 60) == False:
            raise ValueError("Waveform number should be in range (0, 60].")

        if len(val_lst) != 2048:
            raise ValueError("Value list should be exactly 2048 points long.")

        if (min(val_lst) < 0) or (max(val_lst) > 4095):
            raise ValueError("Values should be in range [0, 4095].")

        self.__write("a", waveform_num, val_lst)

        return
