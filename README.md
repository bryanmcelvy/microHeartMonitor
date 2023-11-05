# μHeartMonitor: An ECG-based Heart Rate Monitor

An electrocardiogram-based heart rate monitor project implemented with a TM4C123 microcontroller and custom analog front-end circuitry. Please read below for more information on what exactly this project is and why I decided to do it!

## Navigation

| Directory               | Description                                          |
| ----------------------- | ---------------------------------------------------- |
| [`/cmake`](cmake)       | CMake-specific files for generating the build system |
| [`/docs`](docs)         | Project documentation and external resources         |
| [`/external`](external) | Third-party software used in this project            |
| [`/src`](src)           | Source code                                          |
| [`/test`](test)         | CppUTest-based unit test suite                       |
| [`/tools`](tools)       | Miscellaneous tools used or created for this project |

## Summary

### What is this?
μHeartMonitor is a personal project that I made to increase my experience in embedded software engineering and apply my previous coursework in biomedical engineering. Essentially, it's a fully-functional, ECG-based heart rate monitor that runs on the popular Tiva LaunchPad evaluation kit for the TM4C123 microcontroller.

### How does it work?
An electrocardiogram (or ECG) is a plot of the electrical activity of the heart over time. The hardware in this project senses a patient's ECG signal and transmits it to the microcontroller circuit. The microcontroller receives, processes, and analyzes this signal to determine the average heart rate throughout the previous block of runtime, and outputs both the waveform and the most recently calculated average heart rate to a liquid crystal display (LCD).

### How is this repository organized?
The [Navigation](#navigating-the-repository) section goes into a bit more detail, but the vast majority of the project-specific source code is within the [`/src`](/src) directory. [`main.c`](/src/main.c) is the primary script for the project that is loaded on the microcontroller and implements the heart monitor. The other folders in `/src` host different modules (i.e. collections of `*.c` and `*.h` files) that each implement a particular aspect of the project.

The [`/docs`](/docs/) directory hosts the project's [reference manual](/docs/refman.pdf), the diagrams explained within the HTML/PDF documentation, and external resources.

The project is primarily built using CMake, which generates Makefiles based on the commands in each of the different `CMakeLists.txt` files present in many of the directories.

### Why'd you do all of this?
***Short version***: Because I was interested in doing it and saw utility in doing so.

***Long version***: See the "Motivation" subsection in the documentation.

## Documentation
**HTML/Github Pages Documentation**: [Link](https://bryanmcelvy.github.io/microHeartMonitor)

**PDF Documentation**: [Link](/docs/refman.pdf)
