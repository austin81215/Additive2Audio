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
    private juce::Timer, 
    private juce::Slider::Listener
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
    juce::MidiKeyboardState keyboardState;
    SynthAudioSource synthAudioSource;
    juce::MidiKeyboardComponent keyboardComponent;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;
    std::array<juce::Slider, numHarmonics> harmonicSliders;
    std::array<juce::Slider, numInharmonics> inharmonicPitchSliders;
    std::array<juce::Slider, numInharmonics> inharmonicLevelSliders;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;
    juce::ComboBox presetBox;
    juce::ComboBox notePosBox;
    juce::Slider noteChangeTimeSlider;
    NotePositions currentPosition = NotePositions::End;


    void timerCallback() override;
    void sliderValueChanged(juce::Slider* slider) override;

    // from tutorial, sets MIDI input to the given index
    void setMidiInput(int index);

    // handler for when a preset is selected
    void presetHandler();

    // handler for when note position changed
    void notePosHandler();

    // updates the GUI harmonic sliders to match the underlying levels in the backend
    void updateHarmonicSliders();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
