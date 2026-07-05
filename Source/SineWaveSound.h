#pragma once
#include <JuceHeader.h>

// just copied from the tutorial, doesn't do much and should probably be renamed
// somewhen
struct SineWaveSound : public juce::SynthesiserSound {
    SineWaveSound() {}
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
