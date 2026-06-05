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
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    juce::dsp::Oscillator<float> osc;
    juce::ADSR env;
    juce::dsp::Gain<float> gain;
};
