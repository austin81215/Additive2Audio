#pragma once
#include <JuceHeader.h>

class SynthAudioSource: public juce::AudioSource
{
    public:
        SynthAudioSource(juce::MidiKeyboardState& keyState);
        void setUsingSineWaveSound();
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void releaseResources() override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
        juce::MidiMessageCollector* getMidiCollector();
    
    private:
        juce::MidiKeyboardState& keyboardState;
        juce::Synthesiser synth;
        juce::MidiMessageCollector midiCollector;
};
