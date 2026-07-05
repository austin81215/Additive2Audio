#include "MainComponent.h"
#include "SynthAudioSource.h"

//==============================================================================
MainComponent::MainComponent()
    : synthAudioSource(keyboardState),
      keyboardComponent(keyboardState,
                        juce::MidiKeyboardComponent::horizontalKeyboard) {
    // setup styling

    auto background = juce::Colours::dimgrey.darker();
    auto componentForeground = juce::Colours::silver.darker();
    auto componentBackground = componentForeground.darker();
    auto accent = juce::Colours::slateblue;
    auto text = juce::Colours::lavender;

    getLookAndFeel().setColour(juce::ResizableWindow::backgroundColourId,
                               background);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, accent);
    getLookAndFeel().setColour(juce::Slider::trackColourId,
                               componentForeground);
    getLookAndFeel().setColour(juce::Slider::backgroundColourId,
                               componentBackground);
    getLookAndFeel().setColour(juce::Slider::textBoxOutlineColourId,
                               background);
    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, text);

    getLookAndFeel().setColour(juce::ComboBox::backgroundColourId,
                               componentBackground);
    getLookAndFeel().setColour(juce::ComboBox::outlineColourId, background);
    getLookAndFeel().setColour(juce::ComboBox::textColourId, text);

    getLookAndFeel().setColour(juce::Label::textColourId, text);

    // setup GUI components

    addAndMakeVisible(keyboardComponent);
    setAudioChannels(0, 2);
    auto height = 4 * defaultThickHeight + 8 * defaultThinHeight;
    setSize(1300, height);
    startTimer(400);

    addAndMakeVisible(pitchLabel);
    pitchLabel.setText("Pitch", juce::dontSendNotification);
    addAndMakeVisible(startLabel);
    startLabel.setText("Start", juce::dontSendNotification);
    addAndMakeVisible(endLabel);
    endLabel.setText("End", juce::dontSendNotification);

    for (auto i = 0; i < numHarmonics; i++) {
        auto &slider = harmonicPitchSliders[i];
        addAndMakeVisible(slider);
        slider.setRange(0., numHarmonics);
        slider.setNumDecimalPlacesToDisplay(2);
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider.setSkewFactorFromMidPoint(1);
        slider.onValueChange = [this, i, &slider] {
            synthAudioSource.setHarmonicPitch(i, slider.getValue());
        };
    }

    for (auto i = 0; i < numHarmonics; i++) {
        auto &slider = harmonicStartSliders[i];
        addAndMakeVisible(slider);
        slider.setRange(0., 1.);
        slider.setNumDecimalPlacesToDisplay(2);
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider.onValueChange = [this, i, &slider] {
            synthAudioSource.setHarmonicStartLevel(i, slider.getValue());
        };
    }

    for (auto i = 0; i < numHarmonics; i++) {
        auto &slider = harmonicEndSliders[i];
        addAndMakeVisible(slider);
        slider.setRange(0., 1.);
        slider.setNumDecimalPlacesToDisplay(2);
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider.onValueChange = [this, i, &slider] {
            synthAudioSource.setHarmonicEndLevel(i, slider.getValue());
        };
    }

    updateHarmonicSliders();

    for (auto *slider :
         std::array{&attackSlider, &decaySlider, &releaseSlider}) {
        addAndMakeVisible(*slider);
        slider->setRange(0.01, 5);
        slider->setNumDecimalPlacesToDisplay(2);
        slider->setSkewFactorFromMidPoint(1);
        slider->setColour(juce::Slider::textBoxOutlineColourId,
                          background); // idk why this doesn't work in the
                                       // global styling but i'm busy
    }

    addAndMakeVisible(sustainSlider);
    sustainSlider.setRange(0., 1);
    sustainSlider.setNumDecimalPlacesToDisplay(2);
    sustainSlider.setColour(juce::Slider::textBoxOutlineColourId,
                            background); // ^^^

    attackSlider.setValue(synthAudioSource.getAttack(),
                          juce::dontSendNotification);
    attackSlider.onValueChange = [this] {
        synthAudioSource.setAttack(attackSlider.getValue());
    };

    decaySlider.setValue(synthAudioSource.getDecay(),
                         juce::dontSendNotification);
    decaySlider.onValueChange = [this] {
        synthAudioSource.setDecay(decaySlider.getValue());
    };

    sustainSlider.setValue(synthAudioSource.getSustain(),
                           juce::dontSendNotification);
    sustainSlider.onValueChange = [this] {
        synthAudioSource.setSustain(sustainSlider.getValue());
    };

    releaseSlider.setValue(synthAudioSource.getRelease(),
                           juce::dontSendNotification);
    releaseSlider.onValueChange = [this] {
        synthAudioSource.setRelease(releaseSlider.getValue());
    };

    addAndMakeVisible(attackLabel);
    attackLabel.setText("A", juce::dontSendNotification);

    addAndMakeVisible(decayLabel);
    decayLabel.setText("D", juce::dontSendNotification);

    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("S", juce::dontSendNotification);

    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("R", juce::dontSendNotification);

    addAndMakeVisible(presetBox);
    presetBox.addItem("Sine", static_cast<int>(Preset::Sine));
    presetBox.addItem("Saw", static_cast<int>(Preset::Saw));
    presetBox.addItem("Filtered Saw", static_cast<int>(Preset::FilteredSaw));
    presetBox.addItem("Square", static_cast<int>(Preset::Square));
    presetBox.addItem("Classic Organ", static_cast<int>(Preset::Organ));
    presetBox.addItem("Pull Out All Stops", static_cast<int>(Preset::AllStops));
    presetBox.addItem("Bell", static_cast<int>(Preset::Bell));
    presetBox.addItem("Wah", static_cast<int>(Preset::Wah));
    presetBox.addItem("haW", static_cast<int>(Preset::haW));
    presetBox.addItem("Detune Tremolo", static_cast<int>(Preset::Detune));
    presetBox.addItem("Noise", static_cast<int>(Preset::Noise));
    presetBox.setSelectedId(2);
    presetBox.onChange = [this] { presetHandler(); };

    addAndMakeVisible(automationLabel);
    automationLabel.setText("Automation Time", juce::dontSendNotification);

    addAndMakeVisible(noteChangeTimeSlider);
    noteChangeTimeSlider.setRange(0.01, 5);
    noteChangeTimeSlider.setNumDecimalPlacesToDisplay(2);
    noteChangeTimeSlider.setSkewFactorFromMidPoint(1);
    noteChangeTimeSlider.setColour(juce::Slider::textBoxOutlineColourId,
                                   background); // ^^^
    noteChangeTimeSlider.onValueChange = [this] {
        synthAudioSource.setHarmonicChangeTime(noteChangeTimeSlider.getValue());
    };

    addAndMakeVisible(gainLabel);
    gainLabel.setText("Volume", juce::dontSendNotification);

    addAndMakeVisible(gainSlider);
    gainSlider.setRange(0, 32);
    gainSlider.setValue(1);
    gainSlider.setNumDecimalPlacesToDisplay(2);
    gainSlider.setSkewFactorFromMidPoint(1);
    gainSlider.setColour(juce::Slider::textBoxOutlineColourId,
                         background); // ^^^
    gainSlider.onValueChange = [this] {
        synthAudioSource.setGainMult(gainSlider.getValue());
    };

    addAndMakeVisible(midiInputListLabel);
    midiInputListLabel.setText("MIDI Input:", juce::dontSendNotification);
    midiInputListLabel.attachToComponent(&midiInputList, true);
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    addAndMakeVisible(midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
    juce::StringArray midiInputNames;
    for (auto input : midiInputs)
        midiInputNames.add(input.name);
    midiInputList.addItemList(midiInputNames, 1);
    midiInputList.onChange = [this] {
        setMidiInput(midiInputList.getSelectedItemIndex());
    };
    for (auto input : midiInputs) {
        if (deviceManager.isMidiInputDeviceEnabled(input.identifier)) {
            setMidiInput(midiInputs.indexOf(input));
            break;
        }
    }
    if (midiInputList.getSelectedId() == 0)
        setMidiInput(0);
}

MainComponent::~MainComponent() {
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected,
                                  double sampleRate) {
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI
    // thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear
    // the buffer (to prevent the output of random noise)

    synthAudioSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources() {
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()

    synthAudioSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized() {
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    auto area = getLocalBounds();
    auto thinHeight = area.getHeight() / 16;
    auto thickHeight = 2 * thinHeight;
    auto labelWidth = 50;

    layoutWithPadding(midiInputList, area.removeFromTop(thinHeight));
    layoutWithPadding(keyboardComponent, area.removeFromTop(thickHeight));

    auto sliderWidth = (getWidth() - labelWidth) / numHarmonics;
    auto pitchSliderRow = area.removeFromTop(thickHeight);
    auto startSliderRow = area.removeFromTop(thickHeight);
    auto endSliderRow = area.removeFromTop(thickHeight);

    layoutWithPadding(pitchLabel, pitchSliderRow.removeFromLeft(labelWidth));
    layoutWithPadding(startLabel, startSliderRow.removeFromLeft(labelWidth));
    layoutWithPadding(endLabel, endSliderRow.removeFromLeft(labelWidth));

    for (auto i = 0; i < numHarmonics; i++) {
        layoutWithPadding(harmonicPitchSliders[i],
                          pitchSliderRow.removeFromLeft(sliderWidth));
        layoutWithPadding(harmonicStartSliders[i],
                          startSliderRow.removeFromLeft(sliderWidth));
        layoutWithPadding(harmonicEndSliders[i],
                          endSliderRow.removeFromLeft(sliderWidth));
    }

    auto row = area.removeFromTop(thinHeight);
    layoutWithPadding(attackLabel, row.removeFromLeft(labelWidth));
    layoutWithPadding(attackSlider, row);

    row = area.removeFromTop(thinHeight);
    layoutWithPadding(decayLabel, row.removeFromLeft(labelWidth));
    layoutWithPadding(decaySlider, row);

    row = area.removeFromTop(thinHeight);
    layoutWithPadding(sustainLabel, row.removeFromLeft(labelWidth));
    layoutWithPadding(sustainSlider, row);

    row = area.removeFromTop(thinHeight);
    layoutWithPadding(releaseLabel, row.removeFromLeft(labelWidth));
    layoutWithPadding(releaseSlider, row);

    layoutWithPadding(presetBox, area.removeFromTop(thinHeight));

    row = area.removeFromTop(thinHeight);
    layoutWithPadding(automationLabel, row.removeFromLeft(2 * labelWidth));
    layoutWithPadding(noteChangeTimeSlider, row);

    row = area.removeFromTop(thinHeight);
    layoutWithPadding(gainLabel, row.removeFromLeft(2 * labelWidth));
    layoutWithPadding(gainSlider, row);
}

void MainComponent::timerCallback() {
    keyboardComponent.grabKeyboardFocus();
    stopTimer();
}

void MainComponent::setMidiInput(int index) {
    auto list = juce::MidiInput::getAvailableDevices();
    deviceManager.removeMidiInputDeviceCallback(
        list[lastInputIndex].identifier, synthAudioSource.getMidiCollector());
    auto newInput = list[index];
    if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
        deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);
    deviceManager.addMidiInputDeviceCallback(
        newInput.identifier, synthAudioSource.getMidiCollector());
    midiInputList.setSelectedId(index + 1, juce::dontSendNotification);
    lastInputIndex = index;
}

void MainComponent::presetHandler() {
    auto selected = presetBox.getSelectedId();
    if (selected == 0)
        return;
    synthAudioSource.setPreset(static_cast<Preset>(selected));
    updateHarmonicSliders();
}

void MainComponent::updateHarmonicSliders() {
    for (auto i = 0; i < numHarmonics; i++) {
        harmonicPitchSliders[i].setValue(synthAudioSource.getHarmonicPitch(i),
                                         juce::dontSendNotification);
        harmonicStartSliders[i].setValue(
            synthAudioSource.getHarmonicStartLevel(i),
            juce::dontSendNotification);
        harmonicEndSliders[i].setValue(synthAudioSource.getHarmonicEndLevel(i),
                                       juce::dontSendNotification);
    }
}

void MainComponent::layoutWithPadding(juce::Component &component,
                                      juce::Rectangle<int> bounds) {
    bounds.removeFromTop(5);
    bounds.removeFromBottom(5);
    bounds.removeFromLeft(5);
    bounds.removeFromRight(5);
    component.setBounds(bounds);
}
