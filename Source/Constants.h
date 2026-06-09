#pragma once

// have to be macros since used in templates
#define numHarmonics 8
#define numInharmonics 4

const int numVoices = 4;

enum class Preset 
{
    Sine = 1,
    Saw,
    FilteredSaw,
    Square,
    Organ,
    AllStops
};

