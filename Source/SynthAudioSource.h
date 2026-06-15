#pragma once
#include <JuceHeader.h>
#include "Constants.h"

class SynthAudioSource: public juce::AudioSource
{
    public:
        SynthAudioSource(juce::MidiKeyboardState& keyState);

        // from tutorial, clears all the sounds?
        void setUsingSineWaveSound();
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void releaseResources() override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

        // gets the underlying MidiCollector
        juce::MidiMessageCollector* getMidiCollector();

        // sets the given harmonic (where 0 is the fundamental) to the given level (between 0 and 1)
        void setHarmonicLevel(int index, float level, NotePositions pos);

        // sets the given inharmonic to the given level (between 0 and 1)
        void setInharmonicLevel(int index, float level, NotePositions pos);

        // sets the given harmonic to the given pitch, as a multiple of the fundamental (eg. 1.5 would be a 5th higher)
        void setInharmonicPitch(int index, float mult);

        // sets the harmonic levels to the given preset
        void setPreset(Preset preset, NotePositions pos);

        // returns the harmonic levels between 0 and 1
        std::array<float, numHarmonics> getHarmonicLevels(NotePositions pos);
        
        // returns the inharmonic levels between 0 and 1
        std::array<float, numInharmonics> getInharmonicLevels(NotePositions pos);

        // sets the attack time in seconds
        void setAttack(float level);

        // sets the decay time in seconds
        void setDecay(float level);

        // sets the sustain level from 0 to 1
        void setSustain(float level);

        // sets the release time in seconds
        void setRelease(float level);


        // gets the attack time in seconds
        float getAttack();

        // gets the decay time in seconds
        float getDecay();

        // gets the sustain level from 0 to 1
        float getSustain();

        // gets the release time in seconds
        float getRelease();

        // sets the time that it takes to go from the start sound to the end sound
        void setHarmonicChangeTime(float seconds);

    private:
        juce::MidiKeyboardState& keyboardState;
        juce::Synthesiser synth;
        juce::MidiMessageCollector midiCollector;
};
