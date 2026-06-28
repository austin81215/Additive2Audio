#include "AdditiveVoice.h"
#include "SineWaveSound.h"

AdditiveVoice::AdditiveVoice() 
{
    for(auto& harmonic: harmonics)
        harmonic.osc.initialise([](float x){return std::sin(x);});
    env.setParameters(juce::ADSR::Parameters(A, D, S, R));
    gain.setGainLinear(volume);
}

bool AdditiveVoice::canPlaySound (juce::SynthesiserSound* sound) 
{
    return dynamic_cast<SineWaveSound*> (sound) != nullptr;
}

void AdditiveVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) 
{
    midiNote = midiNoteNumber;
    setHarmonicFreqs();
    env.noteOn();
    noteSamplesElapsed = 0;
}

void AdditiveVoice::stopNote (float /*velocity*/, bool allowTailOff) 
{
    env.noteOff();
}

void AdditiveVoice::pitchWheelMoved (int newPitchWheelValue) 
{
    auto bendSemitones = (newPitchWheelValue - 8192) / 8192. * 2.;
    pitchBendMult = std::pow(2., bendSemitones / 12.);
    setHarmonicFreqs();
}

void AdditiveVoice::controllerMoved (int, int) {}

void AdditiveVoice::renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) 
{
    while (--numSamples >= 0) 
    {
        auto elapsedNoteTime = (float)noteSamplesElapsed / sampleRate;
        auto noteProgress = std::min(elapsedNoteTime / harmonicChangeTime, 1.f);
        noteProgress *= noteProgress;

        auto currentSample = 0.0;

        for(auto& harmonic: harmonics)
        {
            auto coeff = noteProgress * harmonic.endLevel + (1 - noteProgress) * harmonic.startLevel;
            currentSample += harmonic.osc.processSample(0.0f) * coeff;
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
    
    for(auto& harmonic: harmonics)
        harmonic.osc.prepare(spec);

    gain.prepare(spec);
    
    env.setSampleRate(sampleRate);

    this->sampleRate = sampleRate;
}

//void AdditiveVoice::setHarmonicLevel(int index, float level, NotePositions pos)
//{
//    if(level < 0 || level > 1)
//        throw std::range_error("harmonic level must be between 0 and 1 inclusive");
//    if(index < 0 || index >= numHarmonics)
//        throw std::range_error("harmonic index out of bounds");
//    switch(pos)
//    {
//        case NotePositions::Start:
//            break;
//            startCoeffs[index] = level;
//        case NotePositions::End:
//            endCoeffs[index] = level;
//            break;
//    }
//}
//
//void AdditiveVoice::setInharmonicLevel(int index, float level, NotePositions pos)
//{
//    if(level < 0 || level > 1)
//        throw std::range_error("harmonic level must be between 0 and 1 inclusive");
//    if(index < 0 || index >= numInharmonics)
//        throw std::range_error("harmonic index out of bounds");
//    switch(pos)
//    {
//        case NotePositions::Start:
//            break;
//            inharmonicStartCoeffs[index] = level;
//        case NotePositions::End:
//            inharmonicEndCoeffs[index] = level;
//            break;
//    }
//}
//
//void AdditiveVoice::setInharmonicPitch(int index, float mult)
//{
//    if(index < 0 || index >= numInharmonics)
//        throw std::range_error("harmonic index out of bounds");
//    inharmonicPitches[index] = mult;
//}
//
void AdditiveVoice::setPreset(Preset preset)
{
    std::array<float, numHarmonics> newStartCoeffs;
    std::array<float, numHarmonics> newEndCoeffs;
    std::array<float, numHarmonics> newPitches;

    switch(preset)
    {
        case Preset::Sine:
            newStartCoeffs = {1, 0, 0, 0, 0, 0, 0, 0};
            newEndCoeffs = {1, 0, 0, 0, 0, 0, 0, 0};
            newPitches = {1, 2, 3, 4, 5, 6, 7, 8};
            break;
        case Preset::Saw:
            newStartCoeffs = {1, 1/2., 1/3., 1/4., 1/5., 1/6., 1/7., 1/8.};
            newEndCoeffs = {1, 1/2., 1/3., 1/4., 1/5., 1/6., 1/7., 1/8.};
            newPitches = {1, 2, 3, 4, 5, 6, 7, 8};
            break;
        case Preset::FilteredSaw:
            newStartCoeffs = {1, 1/2., 1/3., 1/4., 0, 0, 0, 0};
            newEndCoeffs = {1, 1/2., 1/3., 1/4., 0, 0, 0, 0};
            newPitches = {1, 2, 3, 4, 5, 6, 7, 8};
            break;
        case Preset::Square:
            newStartCoeffs = {1, 0, 1/3., 0, 1/5., 0, 1/7., 0};
            newEndCoeffs = {1, 0, 1/3., 0, 1/5., 0, 1/7., 0};
            newPitches = {1, 2, 3, 4, 5, 6, 7, 8};
            break;
        case Preset::Organ:
            newStartCoeffs = {1, 1, 1, 0, 0, 0, 0, 1};
            newEndCoeffs = {1, 1, 1, 0, 0, 0, 0, 1};
            newPitches = {1, 2, 3, 4, 5, 6, 7, 8};
            break;
        case Preset::AllStops:
            newStartCoeffs = {1, 1, 1, 1, 1, 1, 1, 1};
            newEndCoeffs = {1, 1, 1, 1, 1, 1, 1, 1};
            newPitches = {1, 2, 3, 4, 5, 6, 7, 8};
            break;
    }

    for(auto i = 0; i < numHarmonics; i++)
    {
        harmonics[i].startLevel = newStartCoeffs[i];
        harmonics[i].endLevel = newEndCoeffs[i];
        harmonics[i].pitch = newPitches[i];
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

float AdditiveVoice::hz()
{
    return juce::MidiMessage::getMidiNoteInHertz(midiNote) * pitchBendMult;
}

void AdditiveVoice::setHarmonicFreqs()
{
    for(auto& harmonic: harmonics)
        harmonic.osc.setFrequency(hz() * harmonic.pitch, true);
}
