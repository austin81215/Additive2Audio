#pragma once

#include <JuceHeader.h>
#include "SynthAudioSource.h"
#include "Constants.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : 
    public juce::AudioAppComponent, 
    private juce::Timer 
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    const int defaultThinHeight = 50;
    const int defaultThickHeight = 100;

    juce::MidiKeyboardState keyboardState;
    SynthAudioSource synthAudioSource;
    juce::MidiKeyboardComponent keyboardComponent;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;
    std::array<juce::Slider, numHarmonics> harmonicPitchSliders;
    std::array<juce::Slider, numHarmonics> harmonicStartSliders;
    std::array<juce::Slider, numHarmonics> harmonicEndSliders;
    juce::Label pitchLabel;
    juce::Label startLabel;
    juce::Label endLabel;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;
    juce::ComboBox presetBox;
    juce::Slider noteChangeTimeSlider;
    juce::Label automationLabel;

    void timerCallback() override;

    // from tutorial, sets MIDI input to the given index
    void setMidiInput(int index);

    // handler for when a preset is selected
    void presetHandler();

    // updates the GUI harmonic sliders to match the underlying levels in the backend
    void updateHarmonicSliders();

    // sets component's bounds to bounds but with 10px of padding on each side
    void layoutWithPadding(juce::Component& component, juce::Rectangle<int> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
