#pragma once
#include <JuceHeader.h>
#include "Constants.h"

struct AdditiveVoice: public juce::SynthesiserVoice
{

    AdditiveVoice();

    bool canPlaySound (juce::SynthesiserSound* sound) override;

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    void stopNote (float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved (int) override;
    void controllerMoved (int, int) override;

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    
    // sets up the voice with the given parameters, call before using
    void prepare(double sampleRate, int samplesPerBlock, int chanels);

    // sets the given harmonic (where 0 is the fundamental) to the given level (between 0 and 1)
    void setHarmonicLevel(int index, float level);

    // sets the given inharmonic to the given level (between 0 and 1)
    void setInharmonicLevel(int index, float level);

    // sets the given harmonic to the given pitch, as a multiple of the fundamental (eg. 1.5 would be a 5th higher)
    void setInharmonicPitch(int index, float mult);

    // sets the harmonic levels to the given preset
    void setPreset(Preset preset);

    // returns the harmonic levels between 0 and 1
    std::array<float, numHarmonics> getHarmonicLevels();

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

private:
    std::array<juce::dsp::Oscillator<float>, numHarmonics> oscs;
    std::array<juce::dsp::Oscillator<float>, numInharmonics> inharmonicOscs;
    // saw wave for now
    std::array<float, numHarmonics> coeffs = {1, 1/2., 1/3., 1/4., 1/5., 1/6., 1/7., 1/8.};
    std::array<float, numInharmonics> inharmonicPitches = {1, 1, 1, 1};
    std::array<float, numInharmonics> inharmonicCoeffs = {0, 0, 0, 0};
    juce::ADSR env;
    juce::dsp::Gain<float> gain;

    const float A = 0.1;
    const float D = 0.1;
    const float S = 0.5;
    const float R = 0.5;
    const float volume = 1. / (numHarmonics + numInharmonics) / numVoices;
};
