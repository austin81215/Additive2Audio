#pragma once
#include "Constants.h"
#include <JuceHeader.h>

class SynthAudioSource : public juce::AudioSource {
  public:
    SynthAudioSource(juce::MidiKeyboardState &keyState);

    // from tutorial, clears all the sounds?
    void setUsingSineWaveSound();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(
        const juce::AudioSourceChannelInfo &bufferToFill) override;

    // gets the underlying MidiCollector
    juce::MidiMessageCollector *getMidiCollector();

    // sets the given harmonic (where 0 is the fundamental) to the given start
    // level (between 0 and 1)
    void setHarmonicStartLevel(int index, float level);

    // sets the given harmonic (where 0 is the fundamental) to the given end
    // level (between 0 and 1)
    void setHarmonicEndLevel(int index, float level);

    // sets the given harmonic to the given pitch, as a multiple of the
    // fundamental
    void setHarmonicPitch(int index, float mult);

    // gets the start level (0 to 1) of the given harmonic
    float getHarmonicStartLevel(int index);

    // gets the end level (0 to 1) of the given harmonic
    float getHarmonicEndLevel(int index);

    // getst the pitch of the given harmonic, as a multiple of the fundamental
    float getHarmonicPitch(int index);

    // sets the harmonic levels to the given preset
    void setPreset(Preset preset);

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

    // sets the gain multiplier so synth doesn't have to be super quiet. use big
    // numbers with caution.
    void setGainMult(float mult);

  private:
    juce::MidiKeyboardState &keyboardState;
    juce::Synthesiser synth;
    juce::MidiMessageCollector midiCollector;
};
