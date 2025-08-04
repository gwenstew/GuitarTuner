//process audio input from microphone 
#include "AudioInput.h"

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


