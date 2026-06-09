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
        oscs[i].setFrequency(hz * (i + 1), true);

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

void AdditiveVoice::setHarmonicLevel(int index, float level)
{
    if(level < 0 || level > 1)
        throw std::range_error("harmonic level must be between 0 and 1 inclusive");
    coeffs[index] = level;
}

void AdditiveVoice::setPreset(Preset preset)
{
    switch(preset)
    {
        case Preset::Sine:
            coeffs = {1, 0, 0, 0, 0, 0, 0, 0};
            break;
        case Preset::Saw:
            coeffs = {1, 1/2., 1/3., 1/4., 1/5., 1/6., 1/7., 1/8.};
            break;
        case Preset::FilteredSaw:
            coeffs = {1, 1/2., 1/3., 1/4., 0, 0, 0, 0};
            break;
        case Preset::Square:
            coeffs = {1, 0, 1/3., 0, 1/5., 0, 1/7., 0};
            break;
        case Preset::Organ:
            coeffs = {1, 1, 1, 0, 0, 0, 0, 1};
            break;
        case Preset::AllStops:
            coeffs = {1, 1, 1, 1, 1, 1, 1, 1};
            break;
    }
}

void AdditiveVoice::setAttack(float level)
{
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(level, params.decay, params.sustain, params.release));
}

void AdditiveVoice::setDecay(float level)
{
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(params.attack, level, params.sustain, params.release));
}

void AdditiveVoice::setSustain(float level)
{
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(params.attack, params.decay, level, params.release));
}

void AdditiveVoice::setRelease(float level)
{
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(params.attack, params.decay, params.sustain, level));
}

std::array<float, numHarmonics> AdditiveVoice::getHarmonicLevels()
{
    return coeffs;
}

float AdditiveVoice::getAttack()
{
    return env.getParameters().attack;
}

float AdditiveVoice::getDecay()
{
    return env.getParameters().decay;
}

float AdditiveVoice::getSustain()
{
    return env.getParameters().sustain;
}

float AdditiveVoice::getRelease()
{
    return env.getParameters().release;
}


