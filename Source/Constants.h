#pragma once

constexpr int numHarmonics = 16;
constexpr int numVoices = 8;

enum class Preset 
{
    Sine = 1,
    Saw,
    FilteredSaw,
    Square,
    Organ,
    AllStops,
    Bell,
    Wah,
    haW,
    Detune,
    Noise,
};
