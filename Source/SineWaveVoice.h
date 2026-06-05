#pragma once
#include <JuceHeader.h>

struct SineWaveVoice   : public juce::SynthesiserVoice
{
    SineWaveVoice();

    bool canPlaySound (juce::SynthesiserSound* sound) override;

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    void stopNote (float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved (int) override;
    void controllerMoved (int, int) override;

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    void prepare(double sampleRate, int samplesPerBlock, int chanels);

private:
    juce::dsp::Oscillator<float> osc;
    juce::ADSR env;
    juce::dsp::Gain<float> gain;

    const float A = 0.1;
    const float D = 0.1;
    const float S = 0.5;
    const float R = 0.5;
    const float volume = 0.2;
};
