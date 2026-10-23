#pragma once

#include <nlohmann/json.hpp>

#include <array>
#include <filesystem>
#include <string>

namespace BossRC {

// ---------------------------------------------------------------------------
// SYSTEM
// ---------------------------------------------------------------------------
struct Setup
{
    int MemoryNumber = 0;
    int DisplayMode = 6;
    int Contrast = 4;
    int UndoRedo = 0;
    int ExtentCh = 0;
    int Extent1Min = 0;
    int Extent1Max = 98;
    int Extent2Min = 0;
    int Extent2Max = 98;
    int Extent3Min = 0;
    int Extent3Max = 98;
    int Extent4Min = 0;
    int Extent4Max = 98;
    int Extent5Min = 0;
    int Extent5Max = 98;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Setup, MemoryNumber, DisplayMode, Contrast, UndoRedo, ExtentCh, Extent1Min, Extent1Max,
                                                Extent2Min, Extent2Max, Extent3Min, Extent3Max, Extent4Min, Extent4Max, Extent5Min,
                                                Extent5Max);
};

// ---------------------------------------------------------------------------
struct Midi
{
    int RxCtlCh = 0;
    int Omni = 1;
    int RxNoteCh = 9;
    int TxCh = 16;
    int Sync = 0;
    int ClkOut = 1;
    int SyncStart = 1;
    int PcOut = 1;
    int MidiThru = 0;
    int UsbThru = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Midi, RxCtlCh, Omni, RxNoteCh, TxCh, Sync, ClkOut, SyncStart, PcOut, MidiThru, UsbThru);
};

// ---------------------------------------------------------------------------
struct Input
{
    int Phantom = 0;
    int MicOut = 0;
    int InstOut = 0;
    int RhythmOut = 0;
    int MicNs = 50;
    int InstNs = 50;
    int Indicator = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Input, Phantom, MicOut, InstOut, RhythmOut, MicNs, InstNs, Indicator);
};

// ---------------------------------------------------------------------------
// Present dans SYSTEM (<CTL>) et dans chaque memoire (<CTL>)
// ---------------------------------------------------------------------------
struct Ctl
{
    int Pedal1 = 28;
    int Pedal2 = 36;
    int Pedal3 = 27;
    int Ctl1 = 44;
    int Ctl2 = 53;
    int Exp = 13;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Ctl, Pedal1, Pedal2, Pedal3, Ctl1, Ctl2, Exp);
};

// ---------------------------------------------------------------------------
struct Pref
{
    int Pdl1 = 1;
    int Pdl2 = 1;
    int Pdl3 = 1;
    int Ctl1 = 1;
    int Ctl2 = 1;
    int Exp = 1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Pref, Pdl1, Pdl2, Pdl3, Ctl1, Ctl2, Exp);
};

// ---------------------------------------------------------------------------
struct SystemFile
{
    std::string database_name = "RC-500";   // or RC-5
    int database_revision = 0;
    uint32_t database_counter{};

    Setup setup;
    Midi midi;
    Input input;
    Ctl ctl;
    Pref pref;

    static SystemFile load(const std::filesystem::path& p);
    void save(const std::filesystem::path& p) const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SystemFile, database_name, database_revision, database_counter, setup, midi, input, ctl, pref);
};

// ---------------------------------------------------------------------------
// MEMORY
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Nom de memoire : 12 caracteres ASCII (C01..C12), completes par des espaces.
// ---------------------------------------------------------------------------
struct Name
{
    std::string name;

    [[nodiscard]] std::string to_string(int idx) const;
    [[nodiscard]] std::array<int, 12> to_CXX() const;
    static Name From_CXX(const std::array<int, 12>& s);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Name, name);
};

// ---------------------------------------------------------------------------
struct Track
{
    int Rev = 0;
    int PlyLvl = 100;
    int Pan = 50;
    int One = 0;
    int LoopFx = 1;
    int StrtMod = 0;
    int StpMod = 0;
    int Measure = 1;
    int LoopSync = 1;
    int TempoSync = 1;
    int Input = 0;
    int Output = 0;
    int MeasMod = 1;
    int MeasLen = 0;
    int MeasBtLp = 0;
    int RecTmp = 1200;
    int WavStat = 0;
    int WavLen = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Track, Rev, PlyLvl, Pan, One, LoopFx, StrtMod, StpMod, Measure, LoopSync, TempoSync, Input,
                                                Output, MeasMod, MeasLen, MeasBtLp, RecTmp, WavStat, WavLen);
};

// ---------------------------------------------------------------------------
struct Master
{
    int Tempo = 1200;
    int DubMode = 0;
    int RecAction = 1;
    int RecQuantize = 0;
    int AutoRec = 0;
    int AutoRecSens = 50;
    int AutoRecSrc = 0;
    int PlayMode = 0;
    int SinglPlayeChange = 0;
    int FadeTime = 5;
    int AllStart = 0;
    int TrackChain = 0;
    int CurrentTrack = 0;
    int AllTrackSel = 0;
    int Level = 100;
    int LpMod = 0;
    int LpLen = 0;
    int TrkMod = 1;
    int Sync = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Master, Tempo, DubMode, RecAction, RecQuantize, AutoRec, AutoRecSens, AutoRecSrc, PlayMode,
                                                SinglPlayeChange, FadeTime, AllStart, TrackChain, CurrentTrack, AllTrackSel, Level, LpMod,
                                                LpLen, TrkMod, Sync

    );
};

// ---------------------------------------------------------------------------
struct LoopFxCfg
{
    int Sw = 0;
    int FxType = 4;
    int RepeatLength = 3;
    int ShiftShift = 3;
    int ScatterLength = 2;
    int VinylFlickFlick = 50;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LoopFxCfg, Sw, FxType, RepeatLength, ShiftShift, ScatterLength, VinylFlickFlick);
};

// ---------------------------------------------------------------------------
struct Rhythm
{
    int Level = 100;
    int Reverb = 30;
    int Pattern = 0;
    int Variation = 0;
    int VariationChange = 0;
    int Kit = 0;
    int Beat = 2;
    int Fill = 1;
    int Part1 = 1;
    int Part2 = 1;
    int Part3 = 1;
    int Part4 = 0;
    int RecCount = 0;
    int PlayCount = 0;
    int Start = 0;
    int Stop = 1;
    int ToneLow = 10;
    int ToneHigh = 10;
    int State = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Rhythm, Level, Reverb, Pattern, Variation, VariationChange, Kit, Beat, Fill, Part1, Part2,
                                                Part3, Part4, RecCount, PlayCount, Start, Stop, ToneLow, ToneHigh, State

    );
};

// ---------------------------------------------------------------------------
struct Assign
{
    int Sw = 0;
    int Source = 0;
    int SourceMode = 0;
    int Target = 0;
    int TargetMin = 0;
    int TargetMax = 1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Assign, Sw, Source, SourceMode, Target, TargetMin, TargetMax);
};

// ---------------------------------------------------------------------------
struct Memory
{
    Name name;
    std::array<Track, 2> tracks;

    Master master;
    LoopFxCfg loopFx;
    Rhythm rhythm;
    Ctl ctl;

    std::array<Assign, 8> assigns;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Memory, name, tracks, master, loopFx, rhythm, ctl, assigns);
};

// ---------------------------------------------------------------------------
// Fichiers complets
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
struct MemoryFile
{
    std::string database_name = "RC-500";   // or RC-5
    int database_revision = 0;
    uint32_t database_counter{};

    std::array<Memory, 99> memories;

    static MemoryFile load(const std::filesystem::path& p);
    void save(const std::filesystem::path& p) const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MemoryFile, database_name, database_revision, database_counter, memories);
};

// ---------------------------------------------------------------------------
uint32_t readVersion(const std::string& path);
void writeVersion(const std::string& path, uint32_t version);

}   // namespace BossRC
