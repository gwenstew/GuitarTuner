# Guitar Tuner (C++ & JUCE)

A lightweight **desktop guitar tuner** using the [JUCE](https://juce.com/) framework. Runs natively on macOS and detects pitch in real-time using autocorrelation.

---

## Features
 - Real-time pitch detection from microphone input
 - Simple, responsive desktop UI
 - Toggleable start/stop button
 - Accurate within (test)

---

## How it works
    
- Uses **time-domain autocorrelation function** to estimate pitch
- Maps pitch to closest 3 guitar notes
- Sisplays results on screen with a dial to show proximity to desired note

---

### Requirements
- **CMake 3.22+ (tested with 3.31.5)
- **C++20** compatible compiler
- [JUCE](https://github.com/juce-framework/JUCE) (fetched automatically via CPM.cmake)
- macOS (tested on Ventura/Sonoma, Intel & Apple Silicon)

---

### Build Instructions

```bash
   git clone https://github.com/yourusername/guitar-tuner.git
   cd guitar-tuner
    cmake -B build
    cmake --build build
   ./build/GuitarTuner_artefacts/Guitar\ Tuner.app/Contents/MacOS/Guitar\ Tuner
```
---

Tested on:
- macOS Sonoma 14.5
- Apple Silicon (arm64)
- CMake 3.31.5
- Apple Clang 15.0.0
- JUCE 7 (master branch, Aug 2025

---

### Motivation

I wanted to build this project because I disliked the free online tuners and wanted something I can easily access and use with my own machine! My interest intially piqued when I 
learned about using FFT to detect audio frequencies in an embedded systems class. Through trial and error and research I found that a simple autocorrelation function detects pitch
very well and quite fast.

This implementation currently does not filter for noise and is accurate enough to tune my guitar which I am very happy with. In the future I am interested in implementing 
the YIN algorithm and Bitstream autocorrelation function to see how my simple ACF holds up in speed and accuracy.
