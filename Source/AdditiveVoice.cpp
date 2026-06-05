#include "AdditiveVoice.h"
#include "SineWaveSound.h"

AdditiveVoice::AdditiveVoice() 
{
    for(auto& osc: oscs)
        osc.initialise([](float x){
                return std::sin(x);
                });
    env.setParameters(juce::ADSR::Parameters(A, D, S, R));
    gain.setGainLinear(volume);
}

bool AdditiveVoice::canPlaySound (juce::SynthesiserSound* sound) 
{
    return dynamic_cast<SineWaveSound*> (sound) != nullptr;
}

void AdditiveVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) 
{
    auto hz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    for(auto i = 0; i < oscs.size(); i++)
        oscs[i].setFrequency(hz * (i + 1));

    env.noteOn();
}

void AdditiveVoice::stopNote (float /*velocity*/, bool allowTailOff) 
{
    env.noteOff();
}

void AdditiveVoice::pitchWheelMoved (int) {}
void AdditiveVoice::controllerMoved (int, int) {}

void AdditiveVoice::renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) 
{
    while (--numSamples >= 0) 
    {
        auto currentSample = 0.0;
        for(auto i = 0; i < oscs.size(); i++)
            currentSample += oscs[i].processSample(0.0f) * coeffs[i];
        currentSample = gain.processSample(currentSample * env.getNextSample());

        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample (i, startSample, currentSample);
        ++startSample;
    }
}

void AdditiveVoice::prepare(double sampleRate, int samplesPerBlock, int channels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = channels;
    
    for(auto& osc: oscs)
        osc.prepare(spec);

    gain.prepare(spec);
    
    env.setSampleRate(sampleRate);
}
