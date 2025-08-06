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

std::size_t binarySearch(double pitch) {
    //simple binary search of array of notes to find index of closest note (favors low)
    std::size_t mid;
    std::size_t low = 0;
    std::size_t high = notes.size() - 1;

    while (low <= high) {
        //cpp truncates towards 0
        mid = low + (high - low) / 2;

        if (notes[mid].freq >= (pitch - 0.5) || notes[mid].freq <= (pitch + 0.5)) {
            //return if within range
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
   int minLag = int(sampleRate / 1000.0);
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


