#pragma once
#include <array>
#include <vector>

struct Note {
    const char * note;
    double freq;
};

extern const std::array<Note, 49> notes;

int binarySearch(double pitch);

double detectPitchACF(const float* bufferData, int numSamples, double sampleRate);


