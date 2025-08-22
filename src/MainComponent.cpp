//will contain logic for gui component
#include "MainComponent.h"

/* TO DO :
    
    - Noise reduction; unsure best method for this... i think a low-pass or band-pass might help
        maybe butter filter

    - Harmonic Frequencies; havent run into issues with this yet but i think once i inlcude
        more notes than standard tuning problems will ensue.... can maybe implement harmonic product spectrum
*/

MainComponent::MainComponent()
{

    startButton.setButtonText("Start Tuner");
    startButton.setClickingTogglesState(true);
    startButton.setToggleState(false, juce::dontSendNotification);
    
    startButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    startButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    startButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    startButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::grey);


    addAndMakeVisible(startButton);
    startButton.addListener(this);
    
    //standard audio buffer size is 512... which is not large enough to detect frequencies < 100Hz
    //must initialize MEGA BUFFER!
    megaBuffer.setSize(1, megaBufferSize);

    juce::Component::setSize (600, 400);     //set size of display window
    buildScale();
    
}

MainComponent::~MainComponent() {
    stopTimer();
    shutdownAudio();
    startButton.removeListener(this);
}



void MainComponent::buttonClicked(juce::Button* button) 
{
    if (button == &startButton) {
        if (button->getToggleState()){
            setAudioChannels(1,0);
            startTimer(50);
            button->setButtonText("Stop Tuner");
        } else {
            setAudioChannels(0,0);
            stopTimer();
            button->setButtonText("Start Tuner");
        }
        
    }
        
}

void MainComponent::timerCallback() {
    //loading pitch to protect against race conditions
    currentPitch = pitch.load(std::memory_order_relaxed);

    //only repaint if pitch is within proper bounds (protects agaisnt repainting due to noise)
    if (currentPitch <= 70.0 || currentPitch >= 1400.0) return;

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
    megaBuffer.clear();
}

void MainComponent::buildScale()
{
    //builds scale path
    scalePath.clear();

    int width = getWidth();
    int height = getHeight();

    float pi = float(std::numbers::pi);

    //create scale
    float radius = 200.0;
    float centerX = width/2;
    float centerY = height-75;
    float tickLen = 25.0f;
    scalePath.addCentredArc(0.0f, 0.0f, radius, radius, 0.0f, pi/6, 5*pi/6, true);

    //add ticks to scale path
    auto addTick = [&](float angle) {
        //convert polar coords of scale arc to cartesian for tick marks
        juce::Point<float> start (radius*std::cos(angle),  radius*std::sin(angle));
        juce::Point<float> end ((radius-tickLen)*std::cos(angle), (radius-tickLen)*std::sin(angle));
        scalePath.addLineSegment(juce::Line(start, end), 1.0f);
    };

    addTick(pi/3); //left
    addTick(5*pi/3); //right
    addTick(0.0f); //center

    scalePath.applyTransform(juce::AffineTransform::rotation(3*pi/2));
    scalePath.applyTransform(juce::AffineTransform::translation(centerX, centerY));
    
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
    
    juce::FontOptions fontSet ("Menlo", 20.0f, juce::Font::plain);
    g.setFont(fontSet);
    g.setColour (juce::Colours::black);

    int width = getWidth();
    int height = getHeight();

    g.strokePath(scalePath, PathStrokeType(2.5f)); //draw scale
    g.fillEllipse((width/2)-50, height-125, 100.0f, 100.0f); //draw dial circle

    size_t noteLen = notes.size() -1;
    
    juce::String higherNote;
    juce::String lowerNote;
    size_t closestNoteidx;

    if (std::isnan(currentPitch) || std::isinf(currentPitch)) {
        //check for invalid pitch readings
        g.drawText("No pitch detected...", 0, height / 4, width, 40, juce::Justification::centred, true);
        return;
    } else {
        closestNoteidx = size_t(binarySearch(currentPitch));
        juce::String primaryNote = notes[closestNoteidx].note;

        //print primary note
        g.drawMultiLineText(primaryNote, width/2-100, height-325, 200, juce::Justification::centred);

        if (closestNoteidx > 0 ) {
            //extract lower note
            lowerNote = notes[closestNoteidx -1].note;
        } else {
            lowerNote = " ";
        }

        if (closestNoteidx < noteLen) {
            //extract higher note
            higherNote = notes[closestNoteidx +1].note;
        } else {
            higherNote = " ";
        }
        
        //print secondary notes
        g.drawMultiLineText(lowerNote, width/2 -260, height/2, 200, juce::Justification::centredLeft);
        g.drawMultiLineText(higherNote, width/2 + 60, height/2, 200, juce::Justification::centredRight);

        //print pitch frequency
        juce::String pitchString = "Frequency\n" + juce::String(currentPitch) + " Hz";
        g.drawMultiLineText (pitchString, width/2 + 125, 25, 200, juce::Justification::centredTop);
    }

    //dial logic
    
    //input range (notes[closestNoteIdx - 1].freq and notes[closestNoteIdx - 1].freq)
    //output range unsure? either 5pi/3 to pi/3 (positions of ticks) or 5pi/6 pi/6 (length of scale)
    //linear mapping function --> x = ((pitch - lowerNote) / (higherNote-lowerNote)) * (endScale - startScale) + startScale
    float pi = float(std::numbers::pi);
    float dialRadius = 150.0f;
    double lowerNoteFreq = notes[closestNoteidx - 1].freq;
    double higherNoteFreq = notes[closestNoteidx + 1].freq;

    juce::Point<float> dialStart (width/2, height-75);

    float normalizedPitch = ((currentPitch - lowerNoteFreq) / (higherNoteFreq - lowerNoteFreq));
    normalizedPitch = std::min(1.0f, std::max(0.0f, normalizedPitch));

    float targetTheta =  normalizedPitch * (5*pi/3 - pi/3) + pi/3 - pi/2;
    theta = 0.9f * theta + 0.1f * targetTheta; //lerp to smooth dial movement (theta is global)

    juce::Point<float> dialEnd (width/2 - dialRadius*std::cos(theta), height-75 - dialRadius*std::sin(theta));

    juce::Line<float> dial (dialStart,dialEnd);
    g.drawLine(dial, 5.0f);
}

void MainComponent::resized()
{
    //child components init here
    startButton.setBounds(0, 0, 100, 50);
    buildScale();
}
