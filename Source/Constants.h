#pragma once

// has to be a macro since used in a template
#define numHarmonics 8

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

