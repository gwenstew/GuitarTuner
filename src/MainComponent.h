#pragma once
#include <JuceHeader.h>
#include "AudioInput.h"


class MainComponent : public juce::AudioAppComponent
{
public:

    MainComponent();
    ~MainComponent() override;

    juce::AudioBuffer<float> megaBuffer;
    static const int megaBufferSize = 2048;
    int write = 0;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    double mSampleRate = 4100.0;
    double pitch = 0.0;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
