#define NOMINMAX

#include "tempo_evaluator.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <cmath>
#include <vector>

namespace {

// --------------------------------------------------------------------------
constexpr int SR = 44100;
constexpr int WIN = 2048;                         // taille de fenêtre (~46 ms)
constexpr int HOP = 441;                          // pas : exactement 10 ms
constexpr double FRAME_RATE = double(SR) / HOP;   // 100 frames par seconde

constexpr double BPM_MIN = 50.0;
constexpr double BPM_MAX = 210.0;
constexpr double BPM_PRIOR = 120.0;          // tempo "naturel" privilégié
constexpr double PRIOR_SIGMA_OCT = 1.0;      // écart-type de l'a priori, en octaves
constexpr double LOW_CUTOFF_HZ = 150.0;      // séparation grave / reste
constexpr double LOG_COMPRESSION = 1000.0;   // C dans log(1 + C * env)
constexpr int LOCAL_MEAN_HALF = 50;          // demi-fenêtre de la moyenne locale (0,5 s)

constexpr double PI = 3.14159265358979323846;

// ---------------------------------------------------------------------------
// 1. Décodage
// ---------------------------------------------------------------------------
bool loadMono(const std::string& path, std::vector<float>& out)
{
    ma_decoder_config cfg = ma_decoder_config_init(ma_format_f32, 1, SR);
    ma_decoder decoder;
    if (ma_decoder_init_file(path.c_str(), &cfg, &decoder) != MA_SUCCESS)
        return false;

    out.clear();
    constexpr ma_uint64 CHUNK = 65536;
    std::vector<float> buf(CHUNK);
    for (;;) {
        ma_uint64 got = 0;
        const ma_result res = ma_decoder_read_pcm_frames(&decoder, buf.data(), CHUNK, &got);
        out.insert(out.end(), buf.begin(), buf.begin() + got);
        if (res != MA_SUCCESS || got < CHUNK)
            break;   // fin de fichier
    }
    ma_decoder_uninit(&decoder);
    return !out.empty();
}

// ---------------------------------------------------------------------------
// 2. Bandes de fréquences
// ---------------------------------------------------------------------------
void splitBands(const std::vector<float>& x, std::vector<float>& low, std::vector<float>& high)
{
    const double alpha = 1.0 - std::exp(-2.0 * PI * LOW_CUTOFF_HZ / SR);
    low.resize(x.size());
    high.resize(x.size());
    double y = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        y += alpha * (x[i] - y);
        low[i] = static_cast<float>(y);
        high[i] = x[i] - static_cast<float>(y);
    }
}

// ---------------------------------------------------------------------------
// 3. Enveloppe RMS
// ---------------------------------------------------------------------------
std::vector<float> energyEnvelope(const std::vector<float>& x)
{
    std::vector<float> env;
    if (x.size() < size_t(WIN))
        return env;
    for (size_t i = 0; i + WIN <= x.size(); i += HOP) {
        double sum = 0.0;
        for (int j = 0; j < WIN; ++j)
            sum += static_cast<double>(x[i + j]) * x[i + j];
        env.push_back(static_cast<float>(std::sqrt(sum / WIN)));
    }
    return env;
}

// ---------------------------------------------------------------------------
// 4. Force d'onset (dérivée positive du log de l'enveloppe)
// ---------------------------------------------------------------------------
std::vector<float> positiveLogDiff(const std::vector<float>& env)
{
    std::vector<float> d(env.size(), 0.f);
    for (size_t i = 1; i < env.size(); ++i) {
        float a = std::log(1.f + static_cast<float>(LOG_COMPRESSION) * env[i - 1]);
        float b = std::log(1.f + static_cast<float>(LOG_COMPRESSION) * env[i]);
        if (b - a > d[i]) {
            d[i] = b - a;
        }
    }
    return d;
}

void normalizeByMean(std::vector<float>& v)
{
    double mean = 0.0;
    for (float f : v)
        mean += f;
    if (v.empty())
        return;
    mean /= v.size();
    if (mean < 1e-9)
        return;
    for (float& f : v)
        f = float(f / mean);
}

// Soustrait la moyenne locale et ne garde que le positif : ne reste que les vrais pics.
void removeLocalMean(std::vector<float>& v)
{
    const size_t n = v.size();
    std::vector<double> prefix(n + 1, 0.0);
    for (size_t i = 0; i < n; ++i)
        prefix[i + 1] = prefix[i] + v[i];

    std::vector<float> out(n);
    for (size_t i = 0; i < n; ++i) {
        size_t a = i > size_t(LOCAL_MEAN_HALF) ? i - LOCAL_MEAN_HALF : 0;
        size_t b = std::min(n, i + LOCAL_MEAN_HALF + 1);
        double mean = (prefix[b] - prefix[a]) / double(b - a);
        out[i] = std::max(0.f, float(v[i] - mean));
    }
    v.swap(out);
}

std::vector<float> computeOnsetStrength(const std::vector<float>& samples)
{
    std::vector<float> low, high;
    splitBands(samples, low, high);

    std::vector<float> oLow = positiveLogDiff(energyEnvelope(low));
    std::vector<float> oHigh = positiveLogDiff(energyEnvelope(high));
    normalizeByMean(oLow);
    normalizeByMean(oHigh);

    std::vector<float> onset(oLow.size());
    for (size_t i = 0; i < onset.size(); ++i)
        onset[i] = oLow[i] + oHigh[i];

    removeLocalMean(onset);
    return onset;
}

// --------------------------------------------------------------------------
// 5-7. Tempo : autocorrélation + a priori + interpolation + phase
// --------------------------------------------------------------------------
double priorWeight(double bpm)
{
    const double octaves = std::log2(bpm / BPM_PRIOR);
    return std::exp(-0.5 * (octaves / PRIOR_SIGMA_OCT) * (octaves / PRIOR_SIGMA_OCT));
}

// --------------------------------------------------------------------------
BossRc::TempoResult estimateTempo(const float* onset, const size_t n)
{
    BossRc::TempoResult r;
    const int minLag = int(std::floor(FRAME_RATE * 60.0 / BPM_MAX)) - 1;
    const int maxLag = int(std::ceil(FRAME_RATE * 60.0 / BPM_MIN)) + 1;
    if (n < size_t(maxLag) * 3)
        return r;   // signal trop court

    // Autocorrélation normalisée par le nombre de termes
    std::vector<double> ac(size_t(maxLag) + 1, 0.0);
    for (int lag = 0; lag <= maxLag; ++lag) {
        double acc = 0.0;
        for (size_t i = lag; i < n; ++i)
            acc += static_cast<double>(onset[i]) * onset[i - lag];
        ac[lag] = acc / static_cast<double>(n - lag);
    }
    if (ac[0] < 1e-12)
        return r;   // silence

    // Meilleur lag après pondération par l'a priori
    int bestLag = -1;
    double bestScore = 0.0;
    for (int lag = minLag + 1; lag < maxLag; ++lag) {
        const double bpm = 60.0 * FRAME_RATE / lag;
        const double score = ac[lag] * priorWeight(bpm);
        if (score > bestScore) {
            bestScore = score;
            bestLag = lag;
        }
    }
    if (bestLag < 0)
        return r;

    const double y0 = ac[bestLag - 1];
    // Interpolation parabolique (sur l'autocorrélation non pondérée)
    const double y1 = ac[bestLag];
    const double y2 = ac[bestLag + 1];
    const double denom = y0 - 2.0 * y1 + y2;
    double delta = (std::fabs(denom) > 1e-12) ? 0.5 * (y0 - y2) / denom : 0.0;
    delta = std::max(-1.0, std::min(1.0, delta));
    double refinedLag = bestLag + delta;

    r.bpm = 60.0 * FRAME_RATE / refinedLag;
    r.confidence = ac[bestLag] / ac[0];

    // Phase : décalage qui maximise la somme de l'onset sur la grille de pulsations
    const double period = refinedLag;   // en frames
    double bestPhaseSum = -1.0;
    int bestOffset = 0;
    for (int off = 0; off < int(std::ceil(period)); ++off) {
        double sum = 0.0;
        for (double t = off; t < double(n); t += period)
            sum += onset[size_t(t + 0.5) < n ? size_t(t + 0.5) : n - 1];
        if (sum > bestPhaseSum) {
            bestPhaseSum = sum;
            bestOffset = off;
        }
    }
    r.firstBeat = bestOffset / FRAME_RATE;
    r.valid = true;
    return r;
}
}   // namespace

namespace BossRc {

// --------------------------------------------------------------------------
TempoResult TempoEvaluator(const std::string& file)
{
    std::vector<float> samples;
    if (!loadMono(file, samples)) {
        return {};
    }

    const auto onset = computeOnsetStrength(samples);

    return estimateTempo(onset.data(), onset.size());
}

}   // namespace BossRc
