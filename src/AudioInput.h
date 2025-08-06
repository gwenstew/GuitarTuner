#pragma once
#include <array>
#include <vector>

struct Note {
    const char * note;
    double freq;
};

extern const std::array<Note, 6> notes;

std::size_t binarySearch(double pitch);

double detectPitchACF(const float* bufferData, int numSamples, double sampleRate);


