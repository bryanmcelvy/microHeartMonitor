# Electrocardiogram-based Heart Rate Monitor (ECG-HRM)
- [Electrocardiogram-based Heart Rate Monitor (ECG-HRM)](#electrocardiogram-based-heart-rate-monitor-ecg-hrm)
  - [Navigation](#navigation)
  - [Introduction](#introduction)
    - [Background](#background)
    - [Motivation](#motivation)
    - [Key Terms](#key-terms)
  - [Materials \& Methods](#materials--methods)
    - [Hardware](#hardware)
    - [Software](#software)
  - [Current Results](#current-results)
  - [To-do](#to-do)
  - [Build Instructions](#build-instructions)
  - [References](#references)

## Navigation
* `/cmake_files` - CMake-specific files for generating the build system.
* `/data` - ECG sample data from the publically available  MIT-BIH Arrhythmia Database.
* `/docs` - Documentation for both the project itself and resources used in creating it.
  * `/app_notes` - Application notes.
  * `/datasheets` - Datasheets for hardware components.
  * `/doxygen_files` - Files used for documentation generation via Doxygen.
  * `/help` - Help text for a few of the command line-based applications used in this project.
  * `/manuals` - q manuals for some of the software used in this project.
* `/external` - External software used in this project.
  * `/CMSIS`
  * `/CMSIS-DSP`
* `/src` - Source code for the software modules written for this project.
  * `/app` - Application-specific modules.
  * `/common` - General-purpose modules used by other modules.
  * `/device` - Device-specific files.
  * `/drivers` - Low-level device drivers for the peripherals used in this project.
  * `/middleware` - Software modules for interfacing with external hardware via device drivers.
  * `/old_or_unused` - Old or unused software modules.
  * `/test` - Scripts used for manual on-target testing.
* `/test` - CppUTest-based unit test suite.
  * `/mocks` - CppUMock-based mock functions used to substitute a module's depenencies during unit tests.
  * `/src` - Source code for unit tests.
  * `stubs` - Hard-coded stub functions used to substitute a module's depenencies during unit tests.
* `/tools` - Miscellaneous tools used or created for this project.
  * `cppcheck` - Suppressions list for Cppcheck.
  * `data` - Original files from MIT-BIH Arrhythmia Database, as well as a Python script to convert them to `csv` files.
  * `filter_design` - Python scripts/notebooks used to design the digital filter used in this project.
  * `JDS6600` - Scripts for interfacing a JDS6600 DDS Signal Generator/Counter.
  * `lookup_table` - Script for generating the lookup table used in the ADC module.
---
## Introduction 
### Background
### Motivation
### Key Terms

## Materials & Methods
### Hardware
### Software

## Current Results

## To-do

## Build Instructions

## References
