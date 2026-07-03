#include "SynthAudioSource.h"
#include "SineWaveSound.h"
#include "AdditiveVoice.h"


SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState): keyboardState(keyState)
{
    for(auto i = 0; i < numVoices; ++i)
        synth.addVoice(new AdditiveVoice());
    synth.addSound(new SineWaveSound()); // doesn't do much, probably fine to stay as sine
}

void SynthAudioSource::setUsingSineWaveSound()
{
    synth.clearSounds();
}

void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate) 
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for(auto i = 0; i < synth.getNumVoices(); ++i)
        if(auto* voice = dynamic_cast<AdditiveVoice*>(synth.getVoice(i)))
            voice->prepare(sampleRate, samplesPerBlockExpected, 1);

    midiCollector.reset(sampleRate);
}

void SynthAudioSource::releaseResources() { }

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    bufferToFill.clearActiveBufferRegion();
    juce::MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
    keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true); 
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples); 
}

juce::MidiMessageCollector* SynthAudioSource::getMidiCollector()
{
    return &midiCollector;
}

void SynthAudioSource::setHarmonicStartLevel(int index, float level)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        // AdditiveVoice is the only voice we use so we can static cast
        static_cast<AdditiveVoice*>(synth.getVoice(i))->harmonics[index].startLevel = level;
}

void SynthAudioSource::setHarmonicEndLevel(int index, float level)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        // AdditiveVoice is the only voice we use so we can static cast
        static_cast<AdditiveVoice*>(synth.getVoice(i))->harmonics[index].endLevel = level;
}

void SynthAudioSource::setHarmonicPitch(int index, float pitch)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        // AdditiveVoice is the only voice we use so we can static cast
        static_cast<AdditiveVoice*>(synth.getVoice(i))->harmonics[index].pitch = pitch;
}

void SynthAudioSource::setPreset(Preset preset)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        static_cast<AdditiveVoice*>(synth.getVoice(i))->setPreset(preset);
}

void SynthAudioSource::setAttack(float level)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        static_cast<AdditiveVoice*>(synth.getVoice(i))->setAttack(level);
}

void SynthAudioSource::setDecay(float level)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        static_cast<AdditiveVoice*>(synth.getVoice(i))->setDecay(level);
}

void SynthAudioSource::setSustain(float level)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        static_cast<AdditiveVoice*>(synth.getVoice(i))->setSustain(level);
}

void SynthAudioSource::setRelease(float level)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        static_cast<AdditiveVoice*>(synth.getVoice(i))->setRelease(level);
}

float SynthAudioSource::getHarmonicStartLevel(int index)
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->harmonics[index].startLevel;
}

float SynthAudioSource::getHarmonicEndLevel(int index)
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->harmonics[index].endLevel;
}

float SynthAudioSource::getHarmonicPitch(int index)
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->harmonics[index].pitch;
}

float SynthAudioSource::getAttack()
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->getAttack();
}

float SynthAudioSource::getDecay()
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->getDecay();
}

float SynthAudioSource::getSustain()
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->getSustain();
}

float SynthAudioSource::getRelease()
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->getRelease();
}

void SynthAudioSource::setHarmonicChangeTime(float seconds)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        static_cast<AdditiveVoice*>(synth.getVoice(i))->harmonicChangeTime = seconds;
}

void SynthAudioSource::setGainMult(float mult)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        static_cast<AdditiveVoice*>(synth.getVoice(i))->gainMult = mult;
}
