#pragma once
#include <JuceHeader.h>
#include "Constants.h"

class SynthAudioSource: public juce::AudioSource
{
    public:
        SynthAudioSource(juce::MidiKeyboardState& keyState);
        void setUsingSineWaveSound();
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void releaseResources() override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
        juce::MidiMessageCollector* getMidiCollector();
        void setHarmonicLevel(int index, float level);
        void setInharmonicLevel(int index, float level);
        void setInharmonicPitch(int index, float mult);
        void setPreset(Preset preset);
        std::array<float, numHarmonics> getHarmonicLevels();
        void setAttack(float level);
        void setDecay(float level);
        void setSustain(float level);
        void setRelease(float level);
        float getAttack();
        float getDecay();
        float getSustain();
        float getRelease();

    private:
        juce::MidiKeyboardState& keyboardState;
        juce::Synthesiser synth;
        juce::MidiMessageCollector midiCollector;
};
