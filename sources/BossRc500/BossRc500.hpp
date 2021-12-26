#pragma once

#include <QComboBox>
#include <QPoint>

#include <string>
#include <vector>
#include <initializer_list>

// --------------------------------------------------------------------------
void AddItemsToComboBox(QComboBox* cb, const std::initializer_list<const char*>& list);

// --------------------------------------------------------------------------
// Set all available items in a menu into a ComboBox
// --------------------------------------------------------------------------
namespace BossRc500 {
using Beat = QPoint;

void NotAvailable(QComboBox* cb);
void OffOn(QComboBox* cb);
void IntegerRange(QComboBox* cb, int min, int max); // min, max included
void DoubleRange(QComboBox* cb, double min, double max, double step = 0.1); // min, max included

inline void Level(QComboBox* cb) { IntegerRange(cb, 0, 200); }
inline void Tempo(QComboBox* cb) { DoubleRange(cb, 40, 300, 0.1); }
inline void Reverb(QComboBox* cb) { IntegerRange(cb, 0, 100); }
void Pan(QComboBox* cb);
void Start(QComboBox* cb);
void Stop(QComboBox* cb);
void Measure(QComboBox* cb);
void Input(QComboBox* cb);
void Output(QComboBox* cb);

void DubMode(QComboBox* cb);
void RecAction(QComboBox* cb);
void Quantize(QComboBox* cb);
void RecSource(QComboBox* cb);
void LoopLength(QComboBox* cb);
void PlayMode(QComboBox* cb);
void SingleChange(QComboBox* cb);
void FadeTime(QComboBox* cb);
void AllStart(QComboBox* cb);
void TrkChain(QComboBox* cb);
void TrkSelect(QComboBox* cb);
void LoopFxType(QComboBox* cb);
void ScatLen(QComboBox* cb);
void ReptLen(QComboBox* cb);
void Shift(QComboBox* cb);

void RhythmPattern(QComboBox* cb);
void RhythmVariation(QComboBox* cb);
void RhythmVarChange(QComboBox* cb);
void RhythmKit(QComboBox* cb);
void RhythmBeat(QComboBox* cb);
void RhythmStart(QComboBox* cb);
void RhythmStop(QComboBox* cb);
void RhythmRecCount(QComboBox* cb);
void RhythmPlayCount(QComboBox* cb);
void RhythmTone(QComboBox* cb);
void ControlPdlCtl(QComboBox* cb);
void ControlExpr(QComboBox* cb);

void AssignSource(QComboBox* cb);
void AssignSourceMode(QComboBox* cb);
void AssignTarget(QComboBox* cb);

inline void RecSens(QComboBox* cb) { IntegerRange(cb, 1, 100); }

void AssignFxControl(QComboBox* cb, const Beat& beat);

}

// --------------------------------------------------------------------------
// Get all available item in a menu (can depend on other params)
// --------------------------------------------------------------------------
namespace BossRc500::Items {
std::vector<std::string> Pan();
std::vector<std::string> Start();
std::vector<std::string> Stop();
std::vector<std::string> Input();
std::vector<std::string> Output();
std::vector<std::string> DubMode();
std::vector<std::string> RecAction();
std::vector<std::string> Quantize();
std::vector<std::string> RecSource();
}
