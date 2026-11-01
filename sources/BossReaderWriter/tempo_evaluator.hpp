#pragma once

#include <string>

namespace BossRc {

// --------------------------------------------------------------------------
struct TempoResult
{
    double bpm = 0.0;
    double confidence = 0.0;   // pic d'autocorrélation / énergie (0..1 environ)
    double firstBeat = 0.0;    // position du premier temps, en secondes (relative au début de l'analyse)
    bool valid = false;
};

// --------------------------------------------------------------------------
TempoResult TempoEvaluator(const std::string& file);

}   // namespace BossRc
