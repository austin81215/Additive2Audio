#pragma once

constexpr int numHarmonics = 8;
constexpr int numInharmonics = 4;
constexpr int numVoices = 8;

enum class Preset 
{
    Sine = 1,
    Saw,
    FilteredSaw,
    Square,
    Organ,
    AllStops
};

