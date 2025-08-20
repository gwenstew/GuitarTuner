//will contain logic for gui component
#include "MainComponent.h"

/* TO DO :

    - figure out juce gui
        - how to display moving dial with changing frequencies
    
    - Noise reduction; unsure best method for this... i think a low-pass or band-pass might help
        maybe butter filter

    - Harmonic Frequencies; havent run into issues with this yet but i think once i inlcude
        more notes than standard tuning problems will ensue.... can maybe implement harmonic product spectrum
*/

MainComponent::MainComponent()
{
    setSize (600, 400);     //set size of display window
    buildScale();
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

        // startTuner.onClick = [this] {
        //timer set up
        startTimer(50);
    //};

}

MainComponent::~MainComponent() {
    stopTimer();
    shutdownAudio();
}

void MainComponent::timerCallback() {
    currentPitch = pitch.load(std::memory_order_relaxed);

    if (currentPitch <= 70.0 || currentPitch == 1000.0) return;

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
    //builds scale as an image
    int width = getWidth();
    int height = getHeight();

    scaleImg = juce::Image(juce::Image::ARGB, width, height, true);
    juce::Graphics g(scaleImg);

    g.fillAll(juce::Colours::white);
    g.setColour (juce::Colours::black);

    float pi = float(std::numbers::pi);

    //iff possible use setbufferedtoimage() for scale and other static gui components
    //create scale
    juce::Path scale;
    float radius = 200.0;
    float centerX = width/2;
    float centerY = height-75;
    float tickLen = 25.0f;
    scale.addCentredArc(0.0f, 0.0f, radius, radius, 0.0f, pi/6, 5*pi/6, true);

    //add ticks to scale path
    auto addTick = [&](float angle) {
        //convert polar coords of scale arc to cartesian for tick marks
        juce::Point<float> start (radius*std::cos(angle),  radius*std::sin(angle));
        juce::Point<float> end ((radius-tickLen)*std::cos(angle), (radius-tickLen)*std::sin(angle));
        scale.addLineSegment(juce::Line(start, end), 1.0f);
    };

    addTick(pi/3);
    addTick(5*pi/3);
    addTick(0.0f);

    scale.applyTransform(juce::AffineTransform::rotation(3*pi/2));
    scale.applyTransform(juce::AffineTransform::translation(centerX, centerY));
    
    //draw scale
    g.strokePath(scale, PathStrokeType(2.5f));
}

void MainComponent::paint (juce::Graphics& g)
{
    g.drawImageAt(scaleImg, 0, 0);

    juce::FontOptions fontSet ("Menlo", 20.0f, juce::Font::plain);
    g.setFont(fontSet);
    g.setColour (juce::Colours::black);

    int width = getWidth();
    int height = getHeight();
    size_t noteLen = notes.size() -1;

    
    juce::String higherNote;
    juce::String lowerNote;

    if (std::isnan(currentPitch) || std::isinf(currentPitch)) {
        g.drawText("No pitch detected...", 0, height / 4, width, 40, juce::Justification::centred, true);
        return;
    } else {
        size_t closestNoteidx = size_t(binarySearch(currentPitch));

        juce::String primaryNote = notes[closestNoteidx].note;
        g.drawText(primaryNote, 0, height-325, width, 40, juce::Justification::centred, true);

        if (closestNoteidx > 0 ) {
            //print higher note
            lowerNote = notes[closestNoteidx -1].note;
        } else {
            lowerNote = " ";
        }

        if (closestNoteidx < noteLen) {
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
