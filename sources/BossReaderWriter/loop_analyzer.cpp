#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <algorithm>
#include <cmath>
#include <fmt/base.h>
#include <iostream>
#include <string>
#include <vector>

// --------------------------------------------------
// Configuration
// --------------------------------------------------

constexpr double TARGET_SAMPLE_RATE = 44100.0;

constexpr double MIN_BPM = 50.0;
constexpr double MAX_BPM = 210.0;

constexpr double BPM_STEP = 0.01;

constexpr int SAMPLE_TOLERANCE = 10;

// --------------------------------------------------
// Structure des signatures rythmiques
// --------------------------------------------------

struct TimeSignature
{
    std::string name;

    // Pulsations par mesure selon l'unité du BPM
    double pulsesPerBar;
};

const std::vector<TimeSignature> signatures = {
    // Noire
    {"4/4", 4.0},
    {"3/4", 3.0},
    {"5/4", 5.0},
    {"2/4", 2.0},

    // Noire pointee
    {"12/8", 4.0},
    {"6/8", 2.0},
    {"9/8", 3.0},

    // Croche
    {"7/8", 7.0},
};

// --------------------------------------------------
// Résultat
// --------------------------------------------------

struct LoopCandidate
{
    std::string signature;

    int measures{};

    double exactBpm{};
    double roundedBpm{};

    double theoreticalFrames{};
    double errorFrames{};
};

// --------------------------------------------------
// Lecture des informations WAV
// --------------------------------------------------

bool readWavInfo(const char* filename, ma_uint64& frameCount, ma_uint32& sampleRate, ma_uint32& channels)
{
    ma_decoder decoder;

    ma_result result = ma_decoder_init_file(filename, nullptr, &decoder);

    if (result != MA_SUCCESS) {
        std::cerr << "Erreur ouverture WAV : " << ma_result_description(result) << std::endl;

        return false;
    }

    sampleRate = decoder.outputSampleRate;
    channels = decoder.outputChannels;

    result = ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount);

    ma_decoder_uninit(&decoder);

    if (result != MA_SUCCESS) {
        std::cerr << "Impossible de lire la longueur du WAV." << std::endl;

        return false;
    }

    return true;
}

// --------------------------------------------------
// Recherche des combinaisons
// --------------------------------------------------

std::vector<LoopCandidate> findLoopCandidates(ma_uint64 frames, double sampleRate, int tolerance = SAMPLE_TOLERANCE)
{
    std::vector<LoopCandidate> results;

    const std::vector measuresList = {1, 2, 4, 8, 16, 32};

    for (const auto& [name, pulsesPerBar] : signatures) {
        for (const int measures : measuresList) {
            // BPM exact correspondant à la durée
            const double exactBpm = sampleRate * 60.0 * pulsesPerBar * measures / frames;

            if (exactBpm < MIN_BPM || exactBpm > MAX_BPM) {
                continue;
            }

            // Quantification du BPM
            const double roundedBpm = std::round(exactBpm);

            // fmt::println("Name: {}   -> BPM: {:10.6f}/{:10.6f}, Gap {:10.6f}", name, exactBpm, roundedBpm, ::abs(exactBpm - roundedBpm));
            if (std::abs(exactBpm - roundedBpm) > 0.01) {
                continue;
            }

            // Nombre théorique de frames
            const double theoreticalFrames = sampleRate * 60.0 * pulsesPerBar * measures / roundedBpm;

            // Arrondi au nombre entier de frames
            const double roundedFrames = std::round(theoreticalFrames);

            // fmt::println("            -> Frames: {}/{:f}, rounded frames: {:f}", frames, theoreticalFrames, roundedFrames);
            if (const double errorFrames = std::abs(static_cast<double>(frames) - roundedFrames); errorFrames <= tolerance) {
                results.push_back({.signature = name,
                                   .measures = measures,
                                   .exactBpm = exactBpm,
                                   .roundedBpm = roundedBpm,
                                   .theoreticalFrames = theoreticalFrames,
                                   .errorFrames = errorFrames});
            }
        }
    }

    // Classement par précision
    std::ranges::sort(results, [](const LoopCandidate& a, const LoopCandidate& b) {
        if (a.errorFrames != b.errorFrames)
            return a.errorFrames < b.errorFrames;

        return a.roundedBpm < b.roundedBpm;
    });

    return results;
}

// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Utilisation : loop_analyzer fichier.wav\n";

        return 1;
    }

    ma_uint64 frames = 0;
    ma_uint32 sampleRate = 0;
    ma_uint32 channels = 0;

    if (!readWavInfo(argv[1], frames, sampleRate, channels)) {
        return 1;
    }

    std::cout << "\n--- INFORMATIONS WAV ---\n";
    std::cout << "Frequence : " << sampleRate << " Hz\n";
    std::cout << "Canaux : " << channels << "\n";
    std::cout << "Frames : " << frames << "\n";
    std::cout << "Duree : " << static_cast<double>(frames) / sampleRate << " secondes\n";

    if (sampleRate != TARGET_SAMPLE_RATE) {
        std::cout << "\nAttention : le fichier n'est pas en 44100 Hz.\n";
    }

    std::cout << "\n--- ANALYSE MUSICALE ---\n";

    const auto results = findLoopCandidates(frames, static_cast<double>(sampleRate));

    if (results.empty()) {
        std::cout << "Aucune correspondance dans les criteres.\n";
    }
    else {
        for (const auto& r : results) {
            std::cout << r.signature << " | " << r.measures << " mesures"
                      << " | BPM exact : " << r.exactBpm << " | BPM retenu : " << r.roundedBpm << " | Erreur : " << r.errorFrames
                      << " frames\n";
        }
    }
}
