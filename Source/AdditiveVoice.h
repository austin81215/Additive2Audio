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
    void prepare(double sampleRate, int samplesPerBlock, int chanels);
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
