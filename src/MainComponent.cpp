//will contain logic for gui component
#include "MainComponent.h"

/* TO DO :

    - figure out juce gui
        - how to display sound wave (real time?)

    - implement timer and ISR so that you arent repainting everytime the buffer is emptied
        - may be useful to include logic about current closest notes so you dont have to keep 
            updating every time  
        - seems to be quite fast now (without expenisve gui) maybe interrupt 25-50ms and 
            observe difference in responsiveness; slower responsiveness might help eliminating jumps
            in observed pitch
    
    - Noise reduction; unsure best method for this... i think a low-pass or band-pass might help
        maybe 

    - Harmonic Frequencies; havent run into issues with this yet but i think once i inlcude
        more notes than standard tuning problems will ensue.... can maybe implement harmonic product spectrum
*/

MainComponent::MainComponent()
{
    //timer set up
    startTimer(500);
    
    //standard audio buffer size is 512... which is not large enough to detect frequencies < 100Hz
    //must initialize MEGA BUFFER!
    megaBuffer.setSize(1, megaBufferSize);

    setAudioChannels(1,0);  //set number of input (1) and output channels (0);
    setSize (600, 400);     //set size of display window
}

MainComponent::~MainComponent() {
    stopTimer();
    shutdownAudio();
}

void MainComponent::timerCallback() {
    repaint();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    mSampleRate = sampleRate;
    (void)samplesPerBlockExpected;

}


void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    /*
        Input: address to instance of AudioSourceChannelInfo
              AudioBuffer<float> * buffer
              int numSamples
              int startSample
    */ 
    int numSamples = bufferToFill.numSamples;
    
    if (write >= megaBufferSize ) write = 0;

    //each time buffer is filled we copy it to mega buffer
    megaBuffer.copyFrom(
            0,
            write,
            *bufferToFill.buffer,
            0,
            0,
            numSamples
    );

    write += numSamples;
    
    if (write == megaBufferSize) {
        pitch = detectPitchACF(megaBuffer.getReadPointer(0), megaBufferSize, mSampleRate);
    }


    //update gui with new pitch
    //if (pitch > -1.0) repaint();

}

void MainComponent::releaseResources()
{
    // cleanup audio buffers/resources
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::lightsalmon);

    g.setFont (20.0f);
    g.setColour (juce::Colours::maroon);

    juce::String noteString = notes[binarySearch(pitch)].note;
    g.drawText(noteString, 20, 40, 200, 40, juce::Justification::centred, true);

    juce::String pitchString = juce::String(pitch) + " Hz";
    g.drawText (pitchString, getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions
}
