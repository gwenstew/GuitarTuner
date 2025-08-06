//process audio input from microphone 
#include "AudioInput.h"


const std::array<Note, 6> notes = {{
    {"E2", 82.41},
    {"A2", 110.0},
    {"D3", 146.8},
    {"G3", 196.0},
    {"B3", 246.9},
    {"E4", 329.2}
}};

int binarySearch(double pitch) {
    //simple binary search of array of notes to find index of closest note (favors low)
    int mid;
    int low = 0;
    int high = notes.size() - 1;

    while (low <= high) {
        //cpp truncates towards 0
        mid = low + (high - low) / 2;

        if (notes[mid].freq == pitch) {
            return mid;
        } else if (notes[mid].freq > pitch) {
            //move left
            high = mid - 1;
        } else {
            //move right
            low = mid + 1;
        }
    };
    return low;
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
   int minLag = sampleRate / 1000.0;
   int maxLag = sampleRate / 70.0;

   double maxCorr = 0.0;
   int bestLag = 0;

   //detect silence in the audio signal
   double rms = 0.0;
   for (int i = 0; i<numSamples; i++) {
        rms += bufferData[i] * bufferData[i];
   }
   rms = std::sqrt(rms / numSamples);
   if (rms < 0.01) return -1.0;

   //ACF loop
   for (int lag = minLag; lag < maxLag; lag++) {
        double sum = 0.0;
        for (int x = 0; x < numSamples - lag; x++) {
            //auto correlation sum
            sum += bufferData[x] * bufferData[x + lag];
        }
        if (sum > maxCorr) {
            maxCorr = sum;
            bestLag = lag;
        }
   }

   if (bestLag !=0) {
    return sampleRate / bestLag;
   }

   //return -1 in case of no best lag
   return -1.0;

}


