#include "SynthAudioSource.h"
#include "SineWaveSound.h"
#include "AdditiveVoice.h"

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState): keyboardState(keyState)
{
    for(auto i = 0; i < 4; ++i)
        synth.addVoice(new AdditiveVoice());
    synth.addSound(new SineWaveSound());
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
    keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true); 
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples); 
}

juce::MidiMessageCollector* SynthAudioSource::getMidiCollector()
{
    return &midiCollector;
}

void SynthAudioSource::setHarmonicLevel(int index, float level)
{
    for(auto i = 0; i < synth.getNumVoices(); i++)
        // AdditiveVoice is the only voice we use so we can static cast
        static_cast<AdditiveVoice*>(synth.getVoice(i))->setHarmonicLevel(index, level);
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

std::array<float, 8> SynthAudioSource::getHarmonicLevels()
{
    return static_cast<AdditiveVoice*>(synth.getVoice(0))->getHarmonicLevels();
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

