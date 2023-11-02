@mainpage
@tableofcontents

@section sec_intro Introduction 

@subsection sec_intro_bg Background
***Electrocardiography*** (or ***ECG***) is a diagnostic technique in which the electrical activity of a patient's heart is captured as time series data (AKA the ECG signal) and analyzed to assess cardiovascular health. Specifically, the ECG signal can be analyzed to detect biomarkers for cardiovascular diseases like arrhythmia, myocardial infarction, etc. which manifest as abnormalities in the ECG waveform. In clinical environments, ECG is performed using machines that implement the required hardware and software to acquire, process, and analyze the ECG signal. This must be done in such a way that preserves the important information within the signal (specifically the shape of the ECG waveform) while also maintaining the safety of the patient [1].

The ECG waveform consists of 5 smaller "waves" – the P, Q, R, S, and T waves – that each give information on a patient's cardiac health both individually and collectively. The term ***QRS complex*** refers to the part of the ECG waveform that is generally taken to be the heart "beat". Thus, ECG-based heart rate monitors commonly use a category of algorithms called ***QRS detectors*** to determine the locations of the R-peaks within a block of ECG signal data and calculate the time period between each adjacent peak (i.e. the ***RR interval***) [2]. The RR interval is related to the heart rate by this equation:

\f$
RR = \frac{60}{HR}
\f$

...where \f$RR\f$ is the time in \f$[s]\f$ between two adjacent R peaks, and \f$HR\f$ is the heart rate in \f$[bpm]\f$ (beats per minute).

@htmlonly
<details>
<summary> ❗️ Click to see ECG sample curve ❗️ </summary>
<img src="../../figures/martinek_fig_3.png" width="750" /><br>
Figure 3 from Martinek, et. al. [1]
</details>
@endhtmlonly

@image latex figures/martinek_fig_3.png width=15cm

@htmlonly
The ***μHeartMonitor*** is an embedded system that implements the Pan-Tompkins algorithm for QRS detection. The system consists of both hardware and software that cooperate to achieve this task while also visually outputting the ECG waveform and heart rate to a liquid crystal display (LCD). The text below and the contents of this repository reflect the current progress made, but the end goal is to have the full system mounted on 1-2 printed circuit boards (PCBs) situated inside an insulated enclosure.
@endhtmlonly

@latexonly
The uHeartMonitor is an embedded system that implements the Pan-Tompkins algorithm for QRS detection. The system consists of both hardware and software that cooperate to achieve this task while also visually outputting the ECG waveform and heart rate to a liquid crystal display (LCD). The text below and the contents of this repository reflect the current progress made, but the end goal is to have the full system mounted on 1-2 printed circuit boards (PCBs) situated inside an insulated enclosure.
@endlatexonly

@subsection sec_intro_mot Motivation
My primary motivations for doing this project are:
* Learning more about and gaining exposure to the many different concepts, tools, and challenges involved in embedded systems engineering
* Applying the skills and knowledge I gained from previous coursework, including but not limited to:
  * BIOE 4315: Bioinstrumentation
  * BIOE 4342: Biomedical Signal Processing
  * COSC 2306: Data Programming
  * [Embedded Systems – Shape the World](https://users.ece.utexas.edu/~valvano/Volume1/E-Book/)
* Showing tangible proof of qualification for junior-level embedded software engineering roles to potential employers

I also hope that anyone interested in any of the fields of knowledge relevant to this project (biomedical/electrical/computer/software engineering) will find this helpful to look at or even use in their own projects.

@subsection sec_intro_dis Disclaimer
This project is neither a product nor a medical device (by any legal definition, anyway), and is not intended to be either or both of things now or in the future. It is simply a passion project.

@subsection sec_terms Key Terms
* Electrocardiogram/Electrocardiography (ECG)
* Heart rate
* Heart rate monitor
* QRS complex
* QRS detector
* RR interval

@section sec_meth Materials & Methods

@subsection sec_meth_hard Hardware Design

@htmlonly
<details>
<summary> ❗️ Click to see overall circuit schematic ❗️ </summary>
<img src="../../figures/schematics/circuit_overall.png" width="750" />
</details>
@endhtmlonly

@image latex schematics/circuit_overall.png width=15cm

The hardware is divided into three modules: the analog-front end (AFE), the optical isolation circuit, and the microcontroller/display circuit.

@subsubsection sec_meth_afe Analog-Front End
@htmlonly
<details>
<summary> ❗️ Click to see analog-front end circuit schematic❗️</summary>
<img src="../../figures/schematics/circuit_afe.png" width="750" />
</details>
@endhtmlonly

@image latex schematics/circuit_afe.png width=15cm

The AFE consists of an instrumentation amplifier with a gain of \f$100\f$; a 2nd-order Sallen-Key high-pass filter with a gain of \f$1\f$ and a cutoff frequency of ~\f$0.5\f$ \f$Hz\f$; and a 2nd-order Sallen-Key low-pass filter with a passband gain of \f$11\f$ and a cutoff frequency of ~\f$40\f$ \f$Hz\f$. The overall gain is \f$1100\f$

@subsubsection sec_meth_iso Optical Isolation Circuitry

@htmlonly
<details>
<summary> ❗️ Click to see optical isolation circuit schematic❗️ </summary>
<img src="../../figures/schematics/circuit_isolation.png" width="750" />
</details>
@endhtmlonly

@image latex schematics/circuit_isolation.png width=15cm

The optical isolation circuit uses a linear optocoupler to transmit the ECG signal from the analog-front end circuit to the microcontroller circuit. This circuitry serves as a safety measure against power surges and other potential hazards that can occur as a result of connecting someone directly to mains power (for example, death).

It also has three resistors on the AFE-side that effectively shift the signal from the projected output range of ± \f$5.5\f$ \f$V\f$ to the range \f$[0, 3.5)\f$ \f$V\f$, which is necessary for both the optocoupler and the microcontroller's built-in analog-to-digital converter (ADC) circuitry.

@subsubsection sec_meth_mcu Microcontroller Circuit

@htmlonly
<details>
<summary> ❗️ Click to see microcontroller circuit schematic❗️ </summary>
<img src="../../figures/schematics/circuit_mcu.png" width="750" />
</details>
@endhtmlonly

@image latex schematics/circuit_mcu.png width=15cm

The microcontroller circuit currently consists of a TM4C123 microcontroller mounted on a LaunchPad evaluation kit, and an MSP2807 liquid crystal display (LCD).

@subsection sec_meth_soft Software Architecture

The software has a total of 14 modules, 11 of which are (somewhat loosely) divided into three layers: application-specific software, middleware, and device drivers.
The call graph and data flow graph visually represent the software architecture.

@htmlonly
<details>
<summary> ❗️ Click to see call graph ❗️ </summary>
<img src="../../figures/software/call.png" width="500" />
@endhtmlonly

@image latex software/call.png width=10cm

This graph shows which modules communicate with (or "call") each other. Each arrow points from the "caller" to the "callee".

It also somewhat doubles as an `#include` dependency graph.

@htmlonly
</details>

<details>
<summary> ❗️ Click to see data flow graph ❗️ </summary>
<img src="../../figures/software/data_flow.png" width="750" />
@endhtmlonly

@image latex software/data_flow.png width=12.5cm

This graph shows the flow of information from the patient to the LCD (and also the laptop).

@htmlonly
</details>
@endhtmlonly

@subsubsection sec_meth_device Device Drivers
The device driver layer consists of software modules that interface directly with the microcontroller's built-in peripheral devices.
@see @ref driver

@subsubsection sec_meth_mid Middleware
The middleware layer consists of higher-level device drivers that interface with some hardware connected to one of the built-in peripherals (i.e. the Debug module connects to UART and the ILI9341 module primarily uses SPI).
@see @ref middleware

@subsubsection sec_meth_app Application Software
The application software layer has modules that are at least partially, if not completely built for this project. This layer includes the data acquisition module, whose functions handle receiving raw input samples and denoising them; the QRS detector, which analyzes the filtered signal to determine the average heart rate; and the LCD module, which plots the ECG waveform and displays the heart rate.
@see @ref app

@subsubsection sec_meth_ext External
This "layer" includes modules/libraries/files that were not written (or at least heavily altered) by me. It currently only contains portions of ARM's CMSIS-Core and CMSIS-DSP libraries.

@subsubsection sec_meth_com Common
The "common" modules are general-purpose modules that don't necessarily fit into the above categories/layers. This category includes the "Fifo" module, which contains a ring buffer-based implementation of the FIFO buffer (AKA "queue") data structure; and "NewAssert", which is essentially just an implementation of the `assert` macro that causes a breakpoint (and also doesn't use up as much RAM as the standard implementation does).
@see @ref common

@section sec_results Current Results
Video Demonstration: [YouTube Link](https://youtu.be/KB2CsFbUgtg)

The project is currently implemented using 2 breadboards and a Tiva C LaunchPad development board. The manual tests I've been running use a clone of the JDS6600 signal generator, which I loaded a sample ECG waveform from the MIT-BIH arrhythmia database onto using scripts in the corresponding folder in the `/tools` directory. As can be seen in the video demonstration, the calculated heart rate isn't 100% correct at the moment, but still gets relatively close.

@section sec_todo To-do
@subsection sec_todo_hw Hardware
* Design a custom PCB
  * Replace most of the AFE circuitry with an AFE IC (e.g. AD8232)
  * Add electrostatic discharge (ESD) protection
  * Add decoupling capacitors

@subsection sec_todo_sw Software
* Rework the structure of/relationship between the LCD and ILI9341 modules
* Refactor ADC module to be more general
* Refactor SPI module to be more general
* Remove statically-allocated data structures for unused Timers and GPIO ports
* Add remaining parts of the Pan-Tompkins algorithm
  * Thresholding procedure for bandpass-filtered signal (not just integrated signal)
  * Search-back procedure
  * T-wave discrimination
* Add heart rate variability (HRV) calculation
* Move CMSIS-DSP filters from DAQ and QRS modules to their own module
* Expand the automated test suite

@section sec_build Build Instructions
@subsection sec_build_hw Hardware
WIP

@subsection sec_build_sw Software
WIP

@section sec_ref References

[1] J. Pan and W. J. Tompkins, “A Real-Time QRS Detection Algorithm,” IEEE Trans. Biomed. Eng., vol. BME-32, no. 3, pp. 230–236, Mar. 1985, doi: 10.1109/TBME.1985.325532.

[2] R. Martinek et al., “Advanced Bioelectrical Signal Processing Methods: Past, Present and Future Approach—Part I: Cardiac Signals,” Sensors, vol. 21, no. 15, p. 5186, Jul. 2021, doi: 10.3390/s21155186.

[3] C. Ünsalan, M. E. Yücel, and H. D. Gürhan, Digital Signal Processing using Arm Cortex-M based Microcontrollers: Theory and Practice. Cambridge: ARM Education Media, 2018.

[4] B. B. Winter and J. G. Webster, “Driven-right-leg circuit design,” IEEE Trans Biomed Eng, vol. 30, no. 1, pp. 62–66, Jan. 1983, doi: 10.1109/tbme.1983.325168.

[5] J. Valvano, Embedded Systems: Introduction to ARM Cortex-M Microcontrollers, 5th edition. Jonathan Valvano, 2013.

[6] S. W. Smith, The Scientist and Engineer’s Guide to Digital Signal Processing, 2nd edition. San Diego, Calif: California technical Publishin, 1999.
