//will contain logic for gui component
#include "MainComponent.h"

MainComponent::MainComponent()
{
    setAudioChannels(1,0);  //set number of input (1) and output channels (0);
    setSize (600, 400);     //set size of display window
}

MainComponent::~MainComponent() {
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    mSampleRate = sampleRate;
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
    //pointer to start of buffer
    auto * bufferData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
    pitch = detectPitchACF(bufferData, numSamples, mSampleRate);

    //update gui with new pitch
    repaint();

}

void MainComponent::releaseResources()
{
    // cleanup audio buffers/resources
}

void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);

    juce::String noteString = notes[binarySearch(pitch)].note;
    g.drawText(noteString, 20, 40, 200, 40, juce::Justification::centred, true);

    juce::String pitchString = juce::String(pitch) + " Hz";
    g.drawText (pitchString, getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
