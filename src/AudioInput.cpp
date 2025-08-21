//process audio input from microphone 
#include "AudioInput.h"


const std::array<Note, 49> notes = {{
    {"E2", 82.41},
    {"F2", 87.31},
    {"F#2/Gb2", 92.50},
    {"G2", 98.00},
    {"G#2/Ab2", 103.83},
    {"A2", 110.00},
    {"A#2/Bb2", 116.54},
    {"B2", 123.47},

    {"C3", 130.81},
    {"C#3/Db3", 138.59},
    {"D3", 146.83},
    {"D#3/Eb3", 155.56},
    {"E3", 164.81},
    {"F3", 174.61},
    {"F#3/Gb3", 185.00},
    {"G3", 196.00},
    {"G#3/Ab3", 207.65},
    {"A3", 220.00},
    {"A#3/Bb3", 233.08},
    {"B3", 246.94},

    {"C4", 261.63},
    {"C#4/Db4", 277.18},
    {"D4", 293.66},
    {"D#4/Eb4", 311.13},
    {"E4", 329.63},
    {"F4", 349.23},
    {"F#4/Gb4", 369.99},
    {"G4", 392.00},
    {"G#4/Ab4", 415.30},
    {"A4", 440.00},
    {"A#4/Bb4", 466.16},
    {"B4", 493.88},

    {"C5", 523.25},
    {"C#5/Db5", 554.37},
    {"D5", 587.33},
    {"D#5/Eb5", 622.25},
    {"E5", 659.25},
    {"F5", 698.46},
    {"F#5/Gb5", 739.99},
    {"G5", 783.99},
    {"G#5/Ab5", 830.61},
    {"A5", 880.00},
    {"A#5/Bb5", 932.33},
    {"B5", 987.77},

    {"C6", 1046.50},
    {"C#6/Db6", 1108.73},
    {"D6", 1174.66},
    {"D#6/Eb6", 1244.51},
    {"E6", 1318.51}
}};

int binarySearch(double pitch) {
    //simple binary search of array of notes to find index of closest note (favors low)
    const int n = static_cast<int>(notes.size());
    int low = 0;
    int high = n-1;

    while (low <= high) {
        //cpp truncates towards 0
        int mid = low + (high - low) / 2;

        if (notes[mid].freq >= (pitch - 1) && notes[mid].freq <= (pitch + 1)) {
            //return if within range
            return mid;
        } else if (notes[mid].freq > pitch) {
            //move left
            high = mid - 1;
        } else {
            //move right
            low = mid + 1;
        }
    }
    //if no exact match clamp value between 0 and n-1 or find closest note
    int closest = low;
    if (closest >= n) closest = n - 1;
    if (closest < 0) closest = 0;
    if (closest > 0 && std::abs(notes[size_t(closest) - 1].freq - pitch) < std::abs(notes[size_t(closest)].freq - pitch))
        closest = closest - 1;
    
    return closest;
}

double detectPitchACF(const float* bufferData, int numSamples, double sampleRate) {

    /* Detect pitch function
        Input:
                bufferData: pointer to start of AudioBuffer
                numSamples: size of AudioBuffer
                sampleRate: machine audio sample rate
        Output: 
                current pitch in Hz
    */

    //looking at guitar pitches between 70Hz and 1000Hz
   int minLag = int(sampleRate / 1500.0);
   int maxLag = int(sampleRate / 40.0);

   double maxCorr = 0.0;
   int bestLag = 0;

   //detect silence in the audio signal and compute auto corr at lag 0
   double rms = 0.0;
   double r0 = 0.0;
   for (int i = 0; i<numSamples; i++) {
        rms += bufferData[i] * bufferData[i];
        r0 = rms;
   }
   rms = std::sqrt(rms / numSamples);
   if (rms < 0.001) return -1.0;

   //ACF loop
   
   for (int lag = minLag; lag < maxLag; lag++) {
        double sum = 0.0;
        for (int x = 0; x < numSamples - lag; x++) {
            //auto correlation sum
            sum += bufferData[x] * bufferData[x + lag];
        }

        //compute normalized acf
        double norm = (r0 > 0.0) ? (sum / r0) : 0.0;

        if (norm > maxCorr) {
            maxCorr = norm;
            bestLag = lag;
        }
   }

   if (bestLag !=0) {
    return sampleRate / bestLag;
   }

   //return -1 in case of no best lag
   return -1.0;

}


