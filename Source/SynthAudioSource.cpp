#include "SynthAudioSource.h"
#include "SineWaveSound.h"
#include "SineWaveVoice.h"

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState): keyboardState(keyState)
{
    for(auto i = 0; i < 4; ++i)
        synth.addVoice(new SineWaveVoice());
    synth.addSound(new SineWaveSound());
}

void SynthAudioSource::setUsingSineWaveSound()
{
    synth.clearSounds();
}

void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate) 
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
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
