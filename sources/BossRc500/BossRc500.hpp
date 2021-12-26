#pragma once

#include <QComboBox>
#include <QPoint>

#include <string>
#include <vector>
#include <initializer_list>
#include <functional>

namespace BossRc500 {
// --------------------------------------------------------------------------
using Beat = QPoint;

// --------------------------------------------------------------------------
// Set a Min/Max combo and set current max to the last item
// --------------------------------------------------------------------------
void SetMinMax(QComboBox* min, QComboBox* max, std::function<void(QComboBox*)> fct);

void IntegerRange(QComboBox* cb, int min, int max); // min, max included
void DoubleRange(QComboBox* cb, double min, double max, double step = 0.1); // min, max included

void NotAvailable(QComboBox* cb);
inline void NotAvailableMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, NotAvailable); }

void OffOn(QComboBox* cb);
inline void OffOnMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, OffOn); }

inline void Level(QComboBox* cb) { IntegerRange(cb, 0, 200); }
inline void LevelMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Level); }

inline void Tempo(QComboBox* cb) { DoubleRange(cb, 40, 300, 0.1); }
inline void TempoMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Tempo); }

inline void Reverb(QComboBox* cb) { IntegerRange(cb, 0, 100); }
inline void ReverbMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Reverb); }

void Pan(QComboBox* cb);
inline void PanMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Pan); }

void Start(QComboBox* cb);
inline void StartMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Start); }

void Stop(QComboBox* cb);
inline void StopMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Stop); }

void Measure(QComboBox* cb);

void Input(QComboBox* cb);
inline void InputMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Input); }

void Output(QComboBox* cb);
inline void OutputMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, Output); }

void DubMode(QComboBox* cb);
inline void DubModeMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, DubMode); }

void RecAction(QComboBox* cb);
inline void RecActionMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RecAction); }

void Quantize(QComboBox* cb);

void RecSource(QComboBox* cb);
inline void RecSourceMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RecSource); }

void LoopLength(QComboBox* cb);
inline void LoopLengthMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, LoopLength); }

void PlayMode(QComboBox* cb);
inline void PlayModeMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, PlayMode); }

void SingleChange(QComboBox* cb);
inline void SingleChangeMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, SingleChange); }

void FadeTime(QComboBox* cb);
inline void FadeTimeMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, FadeTime); }

void AllStart(QComboBox* cb);
inline void AllStartMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, AllStart); }

void TrkChain(QComboBox* cb);
inline void TrkChainMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, TrkChain); }

void TrkSelect(QComboBox* cb);
inline void TrkSelectMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, TrkSelect); }

void LoopFxType(QComboBox* cb);
inline void LoopFxTypeMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, LoopFxType); }
bool IsScatter(int value);
bool IsRepeat(int value);
bool IsShift(int value);
bool IsVinyl(int value);

void ScatLen(QComboBox* cb);
void ReptLen(QComboBox* cb);
void Shift(QComboBox* cb);

void ScatLen_EnableItems(QComboBox* cb, const Beat& beat);
void ReptLen_EnableItems(QComboBox* cb, const Beat& beat);
void ShiftLen_EnableItems(QComboBox* cb, const Beat& beat);

void RhythmPattern(QComboBox* cb);
inline void RhythmPatternMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmPattern); }

void RhythmVariation(QComboBox* cb);
inline void RhythmVariationMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmVariation); }

void RhythmVarChange(QComboBox* cb);
inline void RhythmVarChangeMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmVarChange); }

void RhythmKit(QComboBox* cb);
inline void RhythmKitMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmKit); }

void RhythmBeat(QComboBox* cb);

void RhythmStart(QComboBox* cb);
inline void RhythmStartMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmStart); }

void RhythmStop(QComboBox* cb);
inline void RhythmStopMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmStop); }

void RhythmRecCount(QComboBox* cb);
inline void RhythmRecCountMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmRecCount); }

void RhythmPlayCount(QComboBox* cb);
inline void RhythmPlayCountMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmPlayCount); }

void RhythmTone(QComboBox* cb);
inline void RhythmToneMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RhythmTone); }

void ControlPdlCtl(QComboBox* cb);
void ControlExpr(QComboBox* cb);

void AssignSource(QComboBox* cb);
void AssignSourceMode(QComboBox* cb);
void AssignTarget(QComboBox* cb);

inline void RecSens(QComboBox* cb) { IntegerRange(cb, 1, 100); }
inline void RecSensMinMax(QComboBox* min, QComboBox* max) { SetMinMax(min, max, RecSens); }

void AssignFxControl(QComboBox* cb, const Beat& beat, int loopFxType);
inline void AssignFxControlMinMax(QComboBox* min, QComboBox* max, const Beat& beat, int loopFxType)
{
    AssignFxControl(min, beat, loopFxType);
    AssignFxControl(max, beat, loopFxType);
    max->setCurrentIndex(max->count() - 1);

}

inline void AssignMidiMinMax(QComboBox* min, QComboBox* max)
{
    SetMinMax(min, max, [](auto* cb) { IntegerRange(cb, 0, 127); });
}

}
