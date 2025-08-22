#pragma once
#include <numbers>
#include <JuceHeader.h>
#include "AudioInput.h"


class MainComponent : public juce::AudioAppComponent, public juce::Timer, public juce::Button::Listener
{
public:

    MainComponent();
    ~MainComponent() override;

    juce::AudioBuffer<float> megaBuffer;
    static const int megaBufferSize = 2048;
    int write = 0;

    void initializeButton();
    void buttonClicked(juce::Button* button) override;

    void timerCallback() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::TextButton startButton;

    void buildScale();
    juce::Path scalePath;
    juce::Image scaleImg;

    double mSampleRate = 4100.0;
    std::atomic<double> pitch = -1.0;
    double currentPitch = -1.0;
    float theta = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
