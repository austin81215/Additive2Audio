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
