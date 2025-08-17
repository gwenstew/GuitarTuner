WORK IN PROGRESS...

Guitar Tuner desktop app in C++ and JUCE 

I wanted to build this project because I disliked the free online tuners and wanted something I can easily access and use with my own machine! My interest intially piqued when I 
learned about using FFT to detect audio frequencies in an embedded systems class. Through trial and error and research I found that a simple autocorrelation function detects pitch
very well and quite fast.

This implementation currently does not filter for noise and is accurate enough to tune my guitar which I am very happy with. In the future I am interested in implementing 
the YIN algorithm and Bitstream autocorrelation function to see how my simple ACF holds up in speed and accuracy.
