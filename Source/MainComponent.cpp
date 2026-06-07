#include "MainComponent.h"
#include "SynthAudioSource.h"

//==============================================================================
MainComponent::MainComponent(): 
    synthAudioSource(keyboardState),
    keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible (keyboardComponent);
    setAudioChannels (0, 2);
    auto height = 160 + 30 * (4 + numSliders);
    setSize (600, height);
    startTimer (400);

    for(auto& slider: harmonicSliders)
    {
        addAndMakeVisible(slider);
        slider.setRange(0., 1.);
        slider.setNumDecimalPlacesToDisplay(2);
        slider.addListener(this);
    }

    auto harmonicLevels = synthAudioSource.getHarmonicLevels();
    for(auto i = 0; i < numSliders; i++)
        harmonicSliders[i].setValue(harmonicLevels[i], juce::dontSendNotification);

    for(auto* slider: std::array{&attackSlider, &decaySlider, &releaseSlider})
    {
        addAndMakeVisible(*slider);
        slider->setRange(0., 5);
        slider->setNumDecimalPlacesToDisplay(2);
        slider->addListener(this);
    }

    addAndMakeVisible(sustainSlider);
    sustainSlider.setRange(0., 1);
    sustainSlider.setNumDecimalPlacesToDisplay(2);
    sustainSlider.addListener(this);

    attackSlider.setValue(synthAudioSource.getAttack(), juce::dontSendNotification);
    decaySlider.setValue(synthAudioSource.getDecay(), juce::dontSendNotification);
    sustainSlider.setValue(synthAudioSource.getSustain(), juce::dontSendNotification);
    releaseSlider.setValue(synthAudioSource.getRelease(), juce::dontSendNotification);

    addAndMakeVisible(attackLabel);
    attackLabel.setText("A", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, true);
    
    addAndMakeVisible(decayLabel);
    decayLabel.setText("D", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, true);

    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("S", juce::dontSendNotification);
    sustainLabel.attachToComponent(&sustainSlider, true);

    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("R", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, true);

    addAndMakeVisible (midiInputListLabel);
    midiInputListLabel.setText ("MIDI Input:", juce::dontSendNotification);
    midiInputListLabel.attachToComponent (&midiInputList, true);
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    juce::StringArray midiInputNames;
    for (auto input : midiInputs)
        midiInputNames.add (input.name);
    midiInputList.addItemList (midiInputNames, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
    for (auto input : midiInputs)
    {
        if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
        {
            setMidiInput (midiInputs.indexOf (input));
            break;
        }
    }
    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);   
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

    synthAudioSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()

    synthAudioSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    midiInputList.setBounds (10, 10, getWidth() - 20, 20);
    keyboardComponent.setBounds (10, 40, getWidth() - 20, 100);
    for(auto i = 0; i < numSliders; i++)
        harmonicSliders[i].setBounds(10, i * 30 + 150, getWidth() - 20, 20);
    attackSlider.setBounds(20, 400, getWidth() - 30, 20);
    decaySlider.setBounds(20, 430, getWidth() - 30, 20);
    sustainSlider.setBounds(20, 460, getWidth() - 30, 20);
    releaseSlider.setBounds(20, 490, getWidth() - 30, 20);
}

void MainComponent::timerCallback() 
{
    keyboardComponent.grabKeyboardFocus();
    stopTimer();
}

void MainComponent::setMidiInput(int index)
{
    auto list = juce::MidiInput::getAvailableDevices();
    deviceManager.removeMidiInputDeviceCallback (list[lastInputIndex].identifier,
            synthAudioSource.getMidiCollector()); 
    auto newInput = list[index];
    if (!deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);
    deviceManager.addMidiInputDeviceCallback (newInput.identifier, synthAudioSource.getMidiCollector()); 
    midiInputList.setSelectedId (index + 1, juce::dontSendNotification);
    lastInputIndex = index;
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if(slider == &attackSlider)
        synthAudioSource.setAttack(slider->getValue());
    else if(slider == &decaySlider)
        synthAudioSource.setDecay(slider->getValue());
    else if(slider == &sustainSlider)
        synthAudioSource.setSustain(slider->getValue());
    else if(slider == &releaseSlider)
        synthAudioSource.setRelease(slider->getValue());
    else
        for(auto i = 0; i < numSliders; i++)
            if(&(harmonicSliders[i]) == slider)
                synthAudioSource.setHarmonicLevel(i, slider->getValue());
}
