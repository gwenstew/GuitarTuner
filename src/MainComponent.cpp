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
    setSize (600, 400);     //set size of display window

    // startTuner.setButtonText("Start Guitar Tuner"); //set up start button
    // startTuner.setToggleable(1);
    // addAndMakeVisible(startTuner);

   // startTuner.onClick = [this] {
        //timer set up
        startTimer(50);
    
        //standard audio buffer size is 512... which is not large enough to detect frequencies < 100Hz
        //must initialize MEGA BUFFER!
        megaBuffer.setSize(1, megaBufferSize);

        setAudioChannels(1,0);  //set number of input (1) and output channels (0);
        //startTuner.setToggleState(1, dontSendNotification);
    //};

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
    int samplesToCopy = std::min(numSamples, megaBufferSize - write);
    //each time buffer is filled we copy it to mega buffer
    megaBuffer.copyFrom(
            0,
            write,
            *bufferToFill.buffer,
            0,
            0,
            samplesToCopy
    );

    write += samplesToCopy;
    
    if (write >= megaBufferSize) {
        pitch.store(detectPitchACF(megaBuffer.getReadPointer(0), megaBufferSize, mSampleRate), std::memory_order_relaxed);
        megaBuffer.clear();
        write = 0;
    }


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
    int width = getWidth();
    int height = getHeight();
    juce::String higherNote;
    juce::String lowerNote;

    double currentPitch = pitch.load(std::memory_order_relaxed);

    if (currentPitch <= 70.0 || currentPitch == 1000.0|| std::isnan(currentPitch) || std::isinf(currentPitch)) {
        g.drawText("No pitch detected...", 0, height / 4, width, 40, juce::Justification::centred, true);
        return;
    } else {
        size_t closestNoteidx = binarySearch(currentPitch);

        juce::String primaryNote = notes[closestNoteidx].note;
        g.drawText(primaryNote, 0, height / 4, width, 40, juce::Justification::centred, true);

        if (closestNoteidx > 0 ) {
            //print higher note
            lowerNote = notes[closestNoteidx -1].note;
        } else {
            lowerNote = " ";
        }

        if (closestNoteidx < notes.size()-1) {
            //print higher note
            higherNote = notes[closestNoteidx +1].note;
        } else {
            higherNote = " ";
        }
        
        g.drawText(lowerNote, 20, height / 2 + 40, 200, 40, juce::Justification::centredLeft, true);
        g.drawText(higherNote, width - 220, height / 2 + 40, 200, 40, juce::Justification::centredRight, true);

        juce::String pitchString = juce::String(currentPitch) + " Hz";
        g.drawText (pitchString, getLocalBounds(), juce::Justification::centredTop, true);
    }
    
}

void MainComponent::resized()
{
    
}
