#include "SineWaveVoice.h"
#include "SineWaveSound.h"

SineWaveVoice::SineWaveVoice() 
{
    osc.initialise([](float x){
            return std::sin(x);
            });
    env.setParameters(juce::ADSR::Parameters(0.1, 0.1, 0.5, 0.5));
    gain.setGainLinear(0.1);
}

bool SineWaveVoice::canPlaySound (juce::SynthesiserSound* sound) 
{
    return dynamic_cast<SineWaveSound*> (sound) != nullptr;
}

void SineWaveVoice::startNote (int midiNoteNumber, float velocity,
        juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) 
{
    osc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    env.noteOn();
}

void SineWaveVoice::stopNote (float /*velocity*/, bool allowTailOff) 
{
    if (allowTailOff)
    {
        if (tailOff == 0.0)
            tailOff = 1.0;
    }
    else
    {
        clearCurrentNote();
        angleDelta = 0.0;
    }

    env.noteOff();
}

void SineWaveVoice::pitchWheelMoved (int) {}
void SineWaveVoice::controllerMoved (int, int) {}

void SineWaveVoice::renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) 
{
    while (--numSamples >= 0) 
    {
        auto currentSample = gain.processSample(osc.processSample(0.0f) * env.getNextSample());
        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample (i, startSample, currentSample);
        ++startSample;
    }
}

void SineWaveVoice::prepare(double sampleRate, int samplesPerBlock, int channels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = channels;
    
    osc.prepare(spec);
    gain.prepare(spec);
    
    env.setSampleRate(sampleRate);
}
