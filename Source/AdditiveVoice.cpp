#include "AdditiveVoice.h"
#include "SineWaveSound.h"

AdditiveVoice::AdditiveVoice() 
{
    for(auto& osc: oscs)
        osc.initialise([](float x){
                return std::sin(x);
                });
    for(auto& osc: inharmonicOscs)
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
    for(auto i = 0; i < inharmonicOscs.size(); i++)
        inharmonicOscs[i].setFrequency(hz * inharmonicPitches[i], true);

    env.noteOn();
    noteSamplesElapsed = 0;
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
        auto elapsedNoteTime = (float)noteSamplesElapsed / sampleRate;
        auto noteProgress = std::min(elapsedNoteTime / harmonicChangeTime, 1.f);

        auto currentSample = 0.0;

        for(auto i = 0; i < oscs.size(); i++)
        {
            auto coeff = noteProgress * endCoeffs[i] + (1 - noteProgress) * startCoeffs[i];
            currentSample += oscs[i].processSample(0.0f) * coeff;
        }
        
        for(auto i = 0; i < inharmonicOscs.size(); i++)
        {
            auto coeff = noteProgress * inharmonicEndCoeffs[i] + (1 - noteProgress) * inharmonicStartCoeffs[i];
            currentSample += inharmonicOscs[i].processSample(0.0f) * coeff;
        }

        currentSample = gain.processSample(currentSample * env.getNextSample());

        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample (i, startSample, currentSample);
        ++startSample;
        noteSamplesElapsed++;
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

    for(auto& osc: inharmonicOscs)
        osc.prepare(spec);

    gain.prepare(spec);
    
    env.setSampleRate(sampleRate);

    this->sampleRate = sampleRate;
}

void AdditiveVoice::setHarmonicLevel(int index, float level, NotePositions pos)
{
    if(level < 0 || level > 1)
        throw std::range_error("harmonic level must be between 0 and 1 inclusive");
    if(index < 0 || index >= numHarmonics)
        throw std::range_error("harmonic index out of bounds");
    switch(pos)
    {
        case NotePositions::Start:
            break;
            startCoeffs[index] = level;
        case NotePositions::End:
            endCoeffs[index] = level;
            break;
    }
}

void AdditiveVoice::setInharmonicLevel(int index, float level, NotePositions pos)
{
    if(level < 0 || level > 1)
        throw std::range_error("harmonic level must be between 0 and 1 inclusive");
    if(index < 0 || index >= numInharmonics)
        throw std::range_error("harmonic index out of bounds");
    switch(pos)
    {
        case NotePositions::Start:
            break;
            inharmonicStartCoeffs[index] = level;
        case NotePositions::End:
            inharmonicEndCoeffs[index] = level;
            break;
    }
}

void AdditiveVoice::setInharmonicPitch(int index, float mult)
{
    if(index < 0 || index >= numInharmonics)
        throw std::range_error("harmonic index out of bounds");
    inharmonicPitches[index] = mult;
}

void AdditiveVoice::setPreset(Preset preset, NotePositions pos)
{
    std::array<float, numHarmonics> newCoeffs;

    switch(preset)
    {
        case Preset::Sine:
            newCoeffs = {1, 0, 0, 0, 0, 0, 0, 0};
            break;
        case Preset::Saw:
            newCoeffs = {1, 1/2., 1/3., 1/4., 1/5., 1/6., 1/7., 1/8.};
            break;
        case Preset::FilteredSaw:
            newCoeffs = {1, 1/2., 1/3., 1/4., 0, 0, 0, 0};
            break;
        case Preset::Square:
            newCoeffs = {1, 0, 1/3., 0, 1/5., 0, 1/7., 0};
            break;
        case Preset::Organ:
            newCoeffs = {1, 1, 1, 0, 0, 0, 0, 1};
            break;
        case Preset::AllStops:
            newCoeffs = {1, 1, 1, 1, 1, 1, 1, 1};
            break;
    }

    switch(pos)
    {
        case NotePositions::Start:
            startCoeffs = newCoeffs;
            inharmonicStartCoeffs = {0, 0, 0, 0};
            break;
        case NotePositions::End:
            endCoeffs = newCoeffs;
            inharmonicEndCoeffs = {0, 0, 0, 0};
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

std::array<float, numHarmonics> AdditiveVoice::getHarmonicLevels(NotePositions pos)
{
    return pos == NotePositions::Start ? startCoeffs : endCoeffs;
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

void AdditiveVoice::setHarmonicChangeTime(float seconds)
{
    harmonicChangeTime = seconds;
}

std::array<float, numInharmonics> AdditiveVoice::getInharmonicLevels(NotePositions pos)
{
    return pos == NotePositions::Start ? inharmonicStartCoeffs : inharmonicEndCoeffs;
}
