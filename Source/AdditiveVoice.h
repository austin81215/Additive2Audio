#pragma once
#include <JuceHeader.h>

// macro rn since used in a template
#define numHarmonics 4

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

private:
    std::array<juce::dsp::Oscillator<float>, numHarmonics> oscs;
    // saw wave for now
    std::array<float, numHarmonics> coeffs = {1, 1/2., 1/3., 1/4.};
    juce::ADSR env;
    juce::dsp::Gain<float> gain;

    const float A = 0.1;
    const float D = 0.1;
    const float S = 0.5;
    const float R = 0.5;
    const float volume = 0.2;
};
