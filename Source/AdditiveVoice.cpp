#include "AdditiveVoice.h"
#include "SineWaveSound.h"

AdditiveVoice::AdditiveVoice() {
    for (auto &harmonic : harmonics)
        harmonic.osc.initialise([](float x) { return std::sin(x); });
    env.setParameters(juce::ADSR::Parameters(A, D, S, R));
    gain.setGainLinear(maxVolume);
}

bool AdditiveVoice::canPlaySound(juce::SynthesiserSound *sound) {
    return dynamic_cast<SineWaveSound *>(sound) != nullptr;
}

void AdditiveVoice::startNote(int midiNoteNumber, float velocity,
                              juce::SynthesiserSound *,
                              int currentPitchWheelPosition) {
    midiNote = midiNoteNumber;
    gain.setGainLinear(velocity * velocity * maxVolume);
    pitchWheelMoved(currentPitchWheelPosition);
    env.noteOn();
    noteSamplesElapsed = 0;
}

void AdditiveVoice::stopNote(float /*velocity*/, bool allowTailOff) {
    env.noteOff();
}

void AdditiveVoice::pitchWheelMoved(int newPitchWheelValue) {
    auto bendSemitones = (newPitchWheelValue - 8192) / 8192. * 2.;
    pitchBendMult = std::pow(2., bendSemitones / 12.);
    setHarmonicFreqs();
}

void AdditiveVoice::controllerMoved(int, int) {}

void AdditiveVoice::renderNextBlock(juce::AudioSampleBuffer &outputBuffer,
                                    int startSample, int numSamples) {
    while (--numSamples >= 0) {
        auto elapsedNoteTime = (float)noteSamplesElapsed / sampleRate;
        auto noteProgress = std::min(elapsedNoteTime / harmonicChangeTime, 1.f);
        noteProgress *= noteProgress;

        auto currentSample = 0.0;

        for (auto &harmonic : harmonics) {
            auto coeff = noteProgress * harmonic.endLevel +
                         (1 - noteProgress) * harmonic.startLevel;
            currentSample += harmonic.osc.processSample(0.0f) * coeff;
        }

        currentSample =
            gain.processSample(currentSample * env.getNextSample()) * gainMult;

        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample(i, startSample, currentSample);
        ++startSample;
        noteSamplesElapsed++;
    }
}

void AdditiveVoice::prepare(double sampleRate, int samplesPerBlock,
                            int channels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = channels;

    for (auto &harmonic : harmonics)
        harmonic.osc.prepare(spec);

    gain.prepare(spec);

    env.setSampleRate(sampleRate);

    this->sampleRate = sampleRate;
}

// void AdditiveVoice::setHarmonicLevel(int index, float level, NotePositions
// pos)
//{
//     if(level < 0 || level > 1)
//         throw std::range_error("harmonic level must be between 0 and 1
//         inclusive");
//     if(index < 0 || index >= numHarmonics)
//         throw std::range_error("harmonic index out of bounds");
//     switch(pos)
//     {
//         case NotePositions::Start:
//             break;
//             startCoeffs[index] = level;
//         case NotePositions::End:
//             endCoeffs[index] = level;
//             break;
//     }
// }
//
// void AdditiveVoice::setInharmonicLevel(int index, float level, NotePositions
// pos)
//{
//     if(level < 0 || level > 1)
//         throw std::range_error("harmonic level must be between 0 and 1
//         inclusive");
//     if(index < 0 || index >= numInharmonics)
//         throw std::range_error("harmonic index out of bounds");
//     switch(pos)
//     {
//         case NotePositions::Start:
//             break;
//             inharmonicStartCoeffs[index] = level;
//         case NotePositions::End:
//             inharmonicEndCoeffs[index] = level;
//             break;
//     }
// }
//
// void AdditiveVoice::setInharmonicPitch(int index, float mult)
//{
//     if(index < 0 || index >= numInharmonics)
//         throw std::range_error("harmonic index out of bounds");
//     inharmonicPitches[index] = mult;
// }
//

void AdditiveVoice::setPreset(Preset preset) {
    switch (preset) {
    case Preset::Sine:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = i == 0 ? 1 : 0;
            harmonics[i].endLevel = i == 0 ? 1 : 0;
            harmonics[i].pitch = i + 1;
        }
        break;
    case Preset::Saw:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = 1. / (i + 1);
            harmonics[i].endLevel = 1. / (i + 1);
            harmonics[i].pitch = i + 1;
        }
        break;
    case Preset::FilteredSaw:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = i < 4 ? 1. / (i + 1) : 0;
            harmonics[i].endLevel = i < 4 ? 1. / (i + 1) : 0;
            harmonics[i].pitch = i + 1;
        }
        break;
    case Preset::Square:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = i % 2 == 0 ? 1. / (i + 1) : 0;
            harmonics[i].endLevel = i % 2 == 0 ? 1. / (i + 1) : 0;
            harmonics[i].pitch = i + 1;
        }
        break;
    case Preset::Organ:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = i < 3 || i == 7 ? 1 : 0;
            harmonics[i].endLevel = i < 3 || i == 7 ? 1 : 0;
            harmonics[i].pitch = i + 1;
        }
        break;
    case Preset::AllStops:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = 1;
            harmonics[i].endLevel = 1;
            harmonics[i].pitch = i + 1;
        }
        break;
    case Preset::Bell:
        for (auto i = 0; i < numHarmonics - 2; i++) {
            harmonics[i].pitch = i + 1;
            if (i % 2 == 0) {
                harmonics[i].startLevel = 1. / (i + 1);
                harmonics[i].endLevel = 1. / (i + 1);
            } else {
                harmonics[i].startLevel = 0;
                harmonics[i].endLevel = 0;
            }
        }
        harmonics[numHarmonics - 2].startLevel = .75;
        harmonics[numHarmonics - 2].endLevel = .75;
        harmonics[numHarmonics - 2].pitch = 1.2;
        harmonics[numHarmonics - 1].startLevel = .75;
        harmonics[numHarmonics - 1].endLevel = .75;
        harmonics[numHarmonics - 1].pitch = 2.4;
        break;
    case Preset::Wah:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = i < 4 ? 1. / (i + 1) : 0;
            harmonics[i].endLevel = 1. / (i + 1);
            harmonics[i].pitch = i + 1;
            harmonicChangeTime = .2;
        }
        break;
    case Preset::haW:
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = 1. / (i + 1);
            harmonics[i].endLevel = i < 4 ? 1. / (i + 1) : 0;
            harmonics[i].pitch = i + 1;
            harmonicChangeTime = .2;
        }
        break;
    case Preset::Detune:
        for (auto i = 0; i < numHarmonics; i++) {
            const auto detuneAmount = .02;
            harmonics[i].startLevel = 1. / (i / 4 + 1);
            harmonics[i].endLevel = 1. / (i / 4 + 1);
            harmonics[i].pitch =
                (i / 4) + 1 + (i % 4 * detuneAmount - 2 * detuneAmount);
        }
        break;
    case Preset::Noise:
        std::array<float, 16> noise = {
            9.707513,  5.9621167,  6.005609,  5.940589, 2.8377259, 0.09566903,
            8.4352255, 2.2492898,  8.31471,   2.795267, 5.844146,  7.568612,
            9.189848,  0.07325292, 3.1148136, 5.9585714};
        for (auto i = 0; i < numHarmonics; i++) {
            harmonics[i].startLevel = 1;
            harmonics[i].endLevel = 1;
            harmonics[i].pitch = noise[i];
        }
        break;
    }
}

void AdditiveVoice::setAttack(float level) {
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(level, params.decay,
                                             params.sustain, params.release));
}

void AdditiveVoice::setDecay(float level) {
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(params.attack, level,
                                             params.sustain, params.release));
}

void AdditiveVoice::setSustain(float level) {
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(params.attack, params.decay, level,
                                             params.release));
}

void AdditiveVoice::setRelease(float level) {
    auto params = env.getParameters();
    env.setParameters(juce::ADSR::Parameters(params.attack, params.decay,
                                             params.sustain, level));
}

float AdditiveVoice::getAttack() { return env.getParameters().attack; }

float AdditiveVoice::getDecay() { return env.getParameters().decay; }

float AdditiveVoice::getSustain() { return env.getParameters().sustain; }

float AdditiveVoice::getRelease() { return env.getParameters().release; }

float AdditiveVoice::hz() {
    return juce::MidiMessage::getMidiNoteInHertz(midiNote) * pitchBendMult;
}

void AdditiveVoice::setHarmonicFreqs() {
    for (auto &harmonic : harmonics)
        harmonic.osc.setFrequency(hz() * harmonic.pitch, true);
}
