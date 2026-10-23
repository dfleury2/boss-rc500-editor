#include "BossRC.hpp"

#include <fmt/format.h>
#include <pugixml.hpp>

#include <stdexcept>
#include <string_view>
#include <fstream>

namespace {
// --------------------------------------------------------------------------
uint32_t readLE32(const uint8_t* p)
{
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}


// --------------------------------------------------------------------------
void writeLE32(uint8_t* p, uint32_t v)
{
    p[0] = static_cast<uint8_t>(v);
    p[1] = static_cast<uint8_t>(v >> 8);
    p[2] = static_cast<uint8_t>(v >> 16);
    p[3] = static_cast<uint8_t>(v >> 24);
}

// ---------------------------------------------------------------------------
int AsInt(const pugi::xml_node& node, const char* name)
{
    const auto child = node.child(name);
    if (child.empty()) {
        throw std::runtime_error("child " + std::string(name) + " not found for node " + std::string(name));
    }

    const auto text = child.text();
    if (!text) {
        throw std::runtime_error("child " + std::string(name) + " empty");
    }

    return text.as_int();
}

// ---------------------------------------------------------------------------
template <typename NODE>
pugi::xml_node GetNode(const NODE& node, const std::string& path)
{
    const auto child = node.select_node(path.c_str()).node();
    if (!child) {
        throw std::runtime_error("setup " + path + " not found");
    }

    return child;
}

// ---------------------------------------------------------------------------
BossRC::Setup ReadSetup(const pugi::xml_document& doc, const char* path)
{
    const auto node = GetNode(doc, path);

    return {
        .MemoryNumber = AsInt(node, "MemoryNumber"),
        .DisplayMode = AsInt(node, "DisplayMode"),
        .Contrast = AsInt(node, "Contrast"),
        .UndoRedo = AsInt(node, "UndoRedo"),
        .ExtentCh = AsInt(node, "ExtentCh"),
        .Extent1Min = AsInt(node, "Extent1Min"),
        .Extent1Max = AsInt(node, "Extent1Max"),
        .Extent2Min = AsInt(node, "Extent2Min"),
        .Extent2Max = AsInt(node, "Extent2Max"),
        .Extent3Min = AsInt(node, "Extent3Min"),
        .Extent3Max = AsInt(node, "Extent3Max"),
        .Extent4Min = AsInt(node, "Extent4Min"),
        .Extent4Max = AsInt(node, "Extent4Max"),
        .Extent5Min = AsInt(node, "Extent5Min"),
        .Extent5Max = AsInt(node, "Extent5Max"),
    };
}

// ---------------------------------------------------------------------------
void SaveSetup(pugi::xml_node& sys, const BossRC::Setup& setup)
{
    auto node = sys.append_child("SETUP");

    node.append_child("MemoryNumber").text() = setup.MemoryNumber;
    node.append_child("DisplayMode").text() = setup.DisplayMode;
    node.append_child("Contrast").text() = setup.Contrast;
    node.append_child("UndoRedo").text() = setup.UndoRedo;
    node.append_child("ExtentCh").text() = setup.ExtentCh;
    node.append_child("Extent1Min").text() = setup.Extent1Min;
    node.append_child("Extent1Max").text() = setup.Extent1Max;
    node.append_child("Extent2Min").text() = setup.Extent2Min;
    node.append_child("Extent2Max").text() = setup.Extent2Max;
    node.append_child("Extent3Min").text() = setup.Extent3Min;
    node.append_child("Extent3Max").text() = setup.Extent3Max;
    node.append_child("Extent4Min").text() = setup.Extent4Min;
    node.append_child("Extent4Max").text() = setup.Extent4Max;
    node.append_child("Extent5Min").text() = setup.Extent5Min;
    node.append_child("Extent5Max").text() = setup.Extent5Max;
}

// ---------------------------------------------------------------------------
BossRC::Midi ReadMidi(const pugi::xml_document& doc, const char* path)
{
    const auto node = GetNode(doc, path);

    return {
        .RxCtlCh = AsInt(node, "RxCtlCh"),
        .Omni = AsInt(node, "Omni"),
        .RxNoteCh = AsInt(node, "RxNoteCh"),
        .TxCh = AsInt(node, "TxCh"),
        .Sync = AsInt(node, "Sync"),
        .ClkOut = AsInt(node, "ClkOut"),
        .SyncStart = AsInt(node, "SyncStart"),
        .PcOut = AsInt(node, "PcOut"),
        .MidiThru = AsInt(node, "MidiThru"),
        .UsbThru = AsInt(node, "UsbThru"),
    };
}

// ---------------------------------------------------------------------------
void SaveMidi(pugi::xml_node& sys, const BossRC::Midi& midi)
{
    auto node = sys.append_child("MIDI");

    node.append_child("RxCtlCh").text() = midi.RxCtlCh;
    node.append_child("Omni").text() = midi.Omni;
    node.append_child("RxNoteCh").text() = midi.RxNoteCh;
    node.append_child("TxCh").text() = midi.TxCh;
    node.append_child("Sync").text() = midi.Sync;
    node.append_child("ClkOut").text() = midi.ClkOut;
    node.append_child("SyncStart").text() = midi.SyncStart;
    node.append_child("PcOut").text() = midi.PcOut;
    node.append_child("MidiThru").text() = midi.MidiThru;
    node.append_child("UsbThru").text() = midi.UsbThru;
}

// ---------------------------------------------------------------------------
BossRC::Input ReadInput(const pugi::xml_document& doc, const char* path)
{
    const auto node = GetNode(doc, path);

    return {
        .Phantom = AsInt(node, "Phantom"),
        .MicOut = AsInt(node, "MicOut"),
        .InstOut = AsInt(node, "InstOut"),
        .RhythmOut = AsInt(node, "RhythmOut"),
        .MicNs = AsInt(node, "MicNs"),
        .InstNs = AsInt(node, "InstNs"),
        .Indicator = AsInt(node, "Indicator"),
    };
}

// ---------------------------------------------------------------------------
void SaveInput(pugi::xml_node& sys, const BossRC::Input& input)
{
    auto node = sys.append_child("INPUT");

    node.append_child("Phantom").text() = input.Phantom;
    node.append_child("MicOut").text() = input.MicOut;
    node.append_child("InstOut").text() = input.InstOut;
    node.append_child("RhythmOut").text() = input.RhythmOut;
    node.append_child("MicNs").text() = input.MicNs;
    node.append_child("InstNs").text() = input.InstNs;
    node.append_child("Indicator").text() = input.Indicator;
}

// ---------------------------------------------------------------------------
BossRC::Ctl ReadCtl(const pugi::xml_node& node)
{
    return {
        .Pedal1 = AsInt(node, "Pedal1"),
        .Pedal2 = AsInt(node, "Pedal2"),
        .Pedal3 = AsInt(node, "Pedal3"),
        .Ctl1 = AsInt(node, "Ctl1"),
        .Ctl2 = AsInt(node, "Ctl2"),
        .Exp = AsInt(node, "Exp"),
    };
}

// ---------------------------------------------------------------------------
void SaveCtl(pugi::xml_node& sys, const BossRC::Ctl& ctl)
{
    auto node = sys.append_child("CTL");

    node.append_child("Pedal1").text() = ctl.Pedal1;
    node.append_child("Pedal2").text() = ctl.Pedal2;
    node.append_child("Pedal3").text() = ctl.Pedal3;
    node.append_child("Ctl1").text() = ctl.Ctl1;
    node.append_child("Ctl2").text() = ctl.Ctl2;
    node.append_child("Exp").text() = ctl.Exp;
}

// ---------------------------------------------------------------------------
BossRC::Pref ReadPref(const pugi::xml_document& doc, const char* path)
{
    const auto node = GetNode(doc, path);

    return {
        .Pdl1 = AsInt(node, "Pdl1"),
        .Pdl2 = AsInt(node, "Pdl2"),
        .Pdl3 = AsInt(node, "Pdl3"),
        .Ctl1 = AsInt(node, "Ctl1"),
        .Ctl2 = AsInt(node, "Ctl2"),
        .Exp = AsInt(node, "Exp"),
    };
}

// ---------------------------------------------------------------------------
void SavePref(pugi::xml_node& sys, const BossRC::Pref& pref)
{
    auto node = sys.append_child("PREF");

    node.append_child("Pdl1").text() = pref.Pdl1;
    node.append_child("Pdl2").text() = pref.Pdl2;
    node.append_child("Pdl3").text() = pref.Pdl3;
    node.append_child("Ctl1").text() = pref.Ctl1;
    node.append_child("Ctl2").text() = pref.Ctl2;
    node.append_child("Exp").text() = pref.Exp;
}

// ---------------------------------------------------------------------------
BossRC::Name ReadName(const pugi::xml_node& mem_node)
{
    const auto name_node = GetNode(mem_node, "NAME");

    std::array<int, 12> cxx = {};
    for (int i = 1; i <= 12; ++i) {
        cxx[i - 1] = AsInt(name_node, fmt::format("C{:02}", i).c_str());
    }

    return BossRC::Name::From_CXX(cxx);
}

// ---------------------------------------------------------------------------
void SaveName(pugi::xml_node& mem, const BossRC::Name& name, int index)
{
    const auto name_str = name.to_string(index);
    const auto cxx = name.to_CXX();

    auto node = mem.append_child("NAME");
    for (int i = 0; i < cxx.size(); ++i) {
        node.append_child(fmt::format("C{:02}", i + 1).c_str()).text() =
            (i < name_str.size() ? (name_str[i] >= 32 && name_str[i] <= 127 ? name_str[i] : '_') : ' ');
    }
}

// ---------------------------------------------------------------------------
BossRC::Track ReadTrack(const pugi::xml_node& mem_node, const int index)
{
    const auto track_node = GetNode(mem_node, fmt::format("TRACK{}", index).c_str());

    return {
        .Rev = AsInt(track_node, "Rev"),
        .PlyLvl = AsInt(track_node, "PlyLvl"),
        .Pan = AsInt(track_node, "Pan"),
        .One = AsInt(track_node, "One"),
        .LoopFx = AsInt(track_node, "LoopFx"),
        .StrtMod = AsInt(track_node, "StrtMod"),
        .StpMod = AsInt(track_node, "StpMod"),
        .Measure = AsInt(track_node, "Measure"),
        .LoopSync = AsInt(track_node, "LoopSync"),
        .TempoSync = AsInt(track_node, "TempoSync"),
        .Input = AsInt(track_node, "Input"),
        .Output = AsInt(track_node, "Output"),
        .MeasMod = AsInt(track_node, "MeasMod"),
        .MeasLen = AsInt(track_node, "MeasLen"),
        .MeasBtLp = AsInt(track_node, "MeasBtLp"),
        .RecTmp = AsInt(track_node, "RecTmp"),
        .WavStat = AsInt(track_node, "WavStat"),
        .WavLen = AsInt(track_node, "WavLen"),
    };
}

// ---------------------------------------------------------------------------
void SaveTrack(pugi::xml_node& mem, const BossRC::Track& track, int id)
{
    auto node = mem.append_child(fmt::format("TRACK{}", id).c_str());

    node.append_child("Rev").text() = track.Rev;
    node.append_child("PlyLvl").text() = track.PlyLvl;
    node.append_child("Pan").text() = track.Pan;
    node.append_child("One").text() = track.One;
    node.append_child("LoopFx").text() = track.LoopFx;
    node.append_child("StrtMod").text() = track.StrtMod;
    node.append_child("StpMod").text() = track.StpMod;
    node.append_child("Measure").text() = track.Measure;
    node.append_child("LoopSync").text() = track.LoopSync;
    node.append_child("TempoSync").text() = track.TempoSync;
    node.append_child("Input").text() = track.Input;
    node.append_child("Output").text() = track.Output;
    node.append_child("MeasMod").text() = track.MeasMod;
    node.append_child("MeasLen").text() = track.MeasLen;
    node.append_child("MeasBtLp").text() = track.MeasBtLp;
    node.append_child("RecTmp").text() = track.RecTmp;
    node.append_child("WavStat").text() = track.WavStat;
    node.append_child("WavLen").text() = track.WavLen;
}

// ---------------------------------------------------------------------------
BossRC::Master ReadMaster(const pugi::xml_node& mem_node)
{
    const auto master_node = GetNode(mem_node, "MASTER");

    return {
        .Tempo = AsInt(master_node, "Tempo"),
        .DubMode = AsInt(master_node, "DubMode"),
        .RecAction = AsInt(master_node, "RecAction"),
        .RecQuantize = AsInt(master_node, "RecQuantize"),
        .AutoRec = AsInt(master_node, "AutoRec"),
        .AutoRecSens = AsInt(master_node, "AutoRecSens"),
        .AutoRecSrc = AsInt(master_node, "AutoRecSrc"),
        .PlayMode = AsInt(master_node, "PlayMode"),
        .SinglPlayeChange = AsInt(master_node, "SinglPlayeChange"),
        .FadeTime = AsInt(master_node, "FadeTime"),
        .AllStart = AsInt(master_node, "AllStart"),
        .TrackChain = AsInt(master_node, "TrackChain"),
        .CurrentTrack = AsInt(master_node, "CurrentTrack"),
        .AllTrackSel = AsInt(master_node, "AllTrackSel"),
        .Level = AsInt(master_node, "Level"),
        .LpMod = AsInt(master_node, "LpMod"),
        .LpLen = AsInt(master_node, "LpLen"),
        .TrkMod = AsInt(master_node, "TrkMod"),
        .Sync = AsInt(master_node, "Sync"),
    };
}

// ---------------------------------------------------------------------------
void SaveMaster(pugi::xml_node& mem, const BossRC::Master& master)
{
    auto node = mem.append_child("MASTER");

    node.append_child("Tempo").text() = master.Tempo;
    node.append_child("DubMode").text() = master.DubMode;
    node.append_child("RecAction").text() = master.RecAction;
    node.append_child("RecQuantize").text() = master.RecQuantize;
    node.append_child("AutoRec").text() = master.AutoRec;
    node.append_child("AutoRecSens").text() = master.AutoRecSens;
    node.append_child("AutoRecSrc").text() = master.AutoRecSrc;
    node.append_child("PlayMode").text() = master.PlayMode;
    node.append_child("SinglPlayeChange").text() = master.SinglPlayeChange;
    node.append_child("FadeTime").text() = master.FadeTime;
    node.append_child("AllStart").text() = master.AllStart;
    node.append_child("TrackChain").text() = master.TrackChain;
    node.append_child("CurrentTrack").text() = master.CurrentTrack;
    node.append_child("AllTrackSel").text() = master.AllTrackSel;
    node.append_child("Level").text() = master.Level;
    node.append_child("LpMod").text() = master.LpMod;
    node.append_child("LpLen").text() = master.LpLen;
    node.append_child("TrkMod").text() = master.TrkMod;
    node.append_child("Sync").text() = master.Sync;
}

// ---------------------------------------------------------------------------
BossRC::LoopFxCfg ReadLoopFx(const pugi::xml_node& mem_node)
{
    const auto loopfx_node = GetNode(mem_node, "LOOPFX");

    return {
        .Sw = AsInt(loopfx_node, "Sw"),
        .FxType = AsInt(loopfx_node, "FxType"),
        .RepeatLength = AsInt(loopfx_node, "RepeatLength"),
        .ShiftShift = AsInt(loopfx_node, "ShiftShift"),
        .ScatterLength = AsInt(loopfx_node, "ScatterLength"),
        .VinylFlickFlick = AsInt(loopfx_node, "VinylFlickFlick"),
    };
}

// ---------------------------------------------------------------------------
void SaveLoopFx(pugi::xml_node& mem, const BossRC::LoopFxCfg& loopFx)
{
    auto node = mem.append_child("LOOPFX");

    node.append_child("Sw").text() = loopFx.Sw;
    node.append_child("FxType").text() = loopFx.FxType;
    node.append_child("RepeatLength").text() = loopFx.RepeatLength;
    node.append_child("ShiftShift").text() = loopFx.ShiftShift;
    node.append_child("ScatterLength").text() = loopFx.ScatterLength;
    node.append_child("VinylFlickFlick").text() = loopFx.VinylFlickFlick;
}

// ---------------------------------------------------------------------------
BossRC::Rhythm ReadRhythm(const pugi::xml_node& mem_node)
{
    const auto rhythm_node = GetNode(mem_node, "RHYTHM");

    return {
        .Level = AsInt(rhythm_node, "Level"),
        .Reverb = AsInt(rhythm_node, "Reverb"),
        .Pattern = AsInt(rhythm_node, "Pattern"),
        .Variation = AsInt(rhythm_node, "Variation"),
        .VariationChange = AsInt(rhythm_node, "VariationChange"),
        .Kit = AsInt(rhythm_node, "Kit"),
        .Beat = AsInt(rhythm_node, "Beat"),
        .Fill = AsInt(rhythm_node, "Fill"),
        .Part1 = AsInt(rhythm_node, "Part1"),
        .Part2 = AsInt(rhythm_node, "Part2"),
        .Part3 = AsInt(rhythm_node, "Part3"),
        .Part4 = AsInt(rhythm_node, "Part4"),
        .RecCount = AsInt(rhythm_node, "RecCount"),
        .PlayCount = AsInt(rhythm_node, "PlayCount"),
        .Start = AsInt(rhythm_node, "Start"),
        .Stop = AsInt(rhythm_node, "Stop"),
        .ToneLow = AsInt(rhythm_node, "ToneLow"),
        .ToneHigh = AsInt(rhythm_node, "ToneHigh"),
        .State = AsInt(rhythm_node, "State"),
    };
}

// ---------------------------------------------------------------------------
void SaveRhythm(pugi::xml_node& mem, const BossRC::Rhythm& rhythm)
{
    auto node = mem.append_child("RHYTHM");

    node.append_child("Level").text() = rhythm.Level;
    node.append_child("Reverb").text() = rhythm.Reverb;
    node.append_child("Pattern").text() = rhythm.Pattern;
    node.append_child("Variation").text() = rhythm.Variation;
    node.append_child("VariationChange").text() = rhythm.VariationChange;
    node.append_child("Kit").text() = rhythm.Kit;
    node.append_child("Beat").text() = rhythm.Beat;
    node.append_child("Fill").text() = rhythm.Fill;
    node.append_child("Part1").text() = rhythm.Part1;
    node.append_child("Part2").text() = rhythm.Part2;
    node.append_child("Part3").text() = rhythm.Part3;
    node.append_child("Part4").text() = rhythm.Part4;
    node.append_child("RecCount").text() = rhythm.RecCount;
    node.append_child("PlayCount").text() = rhythm.PlayCount;
    node.append_child("Start").text() = rhythm.Start;
    node.append_child("Stop").text() = rhythm.Stop;
    node.append_child("ToneLow").text() = rhythm.ToneLow;
    node.append_child("ToneHigh").text() = rhythm.ToneHigh;
    node.append_child("State").text() = rhythm.State;
}

// ---------------------------------------------------------------------------
BossRC::Assign ReadAssign(const pugi::xml_node& assign_node)
{
    return {
        .Sw = AsInt(assign_node, "Sw"),
        .Source = AsInt(assign_node, "Source"),
        .SourceMode = AsInt(assign_node, "SourceMode"),
        .Target = AsInt(assign_node, "Target"),
        .TargetMin = AsInt(assign_node, "TargetMin"),
        .TargetMax = AsInt(assign_node, "TargetMax"),
    };
}

// ---------------------------------------------------------------------------
void SaveAssign(pugi::xml_node& mem, const BossRC::Assign& assign, int id)
{
    auto node = mem.append_child(fmt::format("ASSIGN{}", id).c_str());

    node.append_child("Sw").text() = assign.Sw;
    node.append_child("Source").text() = assign.Source;
    node.append_child("SourceMode").text() = assign.SourceMode;
    node.append_child("Target").text() = assign.Target;
    node.append_child("TargetMin").text() = assign.TargetMin;
    node.append_child("TargetMax").text() = assign.TargetMax;
}

}   // namespace

namespace BossRC {
// ---------------------------------------------------------------------------
std::string Name::to_string(const int idx) const
{
    return (!name.empty() ? name : fmt::format("Memory{:02}", idx + 1));
}

// ---------------------------------------------------------------------------
std::array<int, 12> Name::to_CXX() const
{
    std::array<int, 12> s = {};

    for (int i = 0; i < s.size(); ++i) {
        s[i] = i < name.size() ? static_cast<int>(name[i]) : ' ';
    }
    return s;
}

// ---------------------------------------------------------------------------
Name Name::From_CXX(const std::array<int, 12>& s)
{
    Name name;
    name.name = "";
    for (const auto i : s) {
        name.name += static_cast<char>(i);
    }
    return name;
}

// --------------------------------------------------------------------------
// Lit l'entier LE32 situé aux 4 derniers octets du fichier.
// --------------------------------------------------------------------------
uint32_t readVersion(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f)
        throw std::runtime_error("Unable to open the file: " + path);

    f.seekg(-4, std::ios::end);
    if (!f)
        throw std::runtime_error("Unable to go to the end of the file");

    uint8_t b[4];
    if (!f.read(reinterpret_cast<char*>(b), 4))
        throw std::runtime_error("Unable to read the last 4 bytes");
    return readLE32(b);
}

// --------------------------------------------------------------------------
// Remplace les 4 derniers octets du fichier par la valeur LE32.
// --------------------------------------------------------------------------
void writeVersion(const std::string& path, const uint32_t version)
{
    std::ofstream f(path, std::ios::binary | std::ios::app);
    if (!f)
        throw std::runtime_error("Unable to open the file: " + path);

    uint8_t b[4] = {};
    writeLE32(b, version);
    f.write(reinterpret_cast<const char*>(b), 4);
}

// ---------------------------------------------------------------------------
SystemFile SystemFile::load(const std::filesystem::path& p)
{
    pugi::xml_document doc;
    if (const pugi::xml_parse_result result = doc.load_file(p.string().c_str()); !result) {
        throw std::runtime_error("Error during reading " + p.string());
    }

    SystemFile system;
    {   // database attributes
        const auto database = doc.child("database");
        system.database_name = database.attribute("name").value();
        system.database_revision = database.attribute("revision").as_int();
        system.database_counter = readVersion(p.string());
    }

    system.setup = ReadSetup(doc, "database/sys/SETUP");
    system.midi = ReadMidi(doc, "database/sys/MIDI");
    system.input = ReadInput(doc, "database/sys/INPUT");
    system.ctl = ReadCtl(GetNode(doc, "database/sys/CTL"));
    system.pref = ReadPref(doc, "database/sys/PREF");

    return system;
}

// ---------------------------------------------------------------------------
void SystemFile::save(const std::filesystem::path& p) const
{
    pugi::xml_document doc;

    pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "utf-8";

    auto database = doc.append_child("database");
    database.append_attribute("name").set_value(database_name.c_str());
    database.append_attribute("revision").set_value(database_revision);

    auto sys = database.append_child("sys");

    SaveSetup(sys, setup);
    SaveMidi(sys, midi);
    SaveInput(sys, input);
    SaveCtl(sys, ctl);
    SavePref(sys, pref);

    if (!doc.save_file(p.string().c_str(), "\t", pugi::xml_encoding::encoding_utf8)) {
        throw std::runtime_error("Error during writing " + p.string());
    }

    writeVersion(p.string(), database_counter);
}

// ---------------------------------------------------------------------------
MemoryFile MemoryFile::load(const std::filesystem::path& p)
{
    pugi::xml_document doc;
    if (const pugi::xml_parse_result result = doc.load_file(p.string().c_str()); !result) {
        throw std::runtime_error("Error during reading " + p.string());
    }

    MemoryFile memory_file;
    {   // database attributes
        const auto database = doc.child("database");
        memory_file.database_name = database.attribute("name").value();
        memory_file.database_revision = database.attribute("revision").as_int();
    }

    int idx = 0;
    for (const auto mem_xpath : doc.select_nodes("database/mem")) {
        const auto mem_node = mem_xpath.node();

        auto& memory = memory_file.memories[idx];

        memory.name = ReadName(mem_node);
        memory.tracks[0] = ReadTrack(mem_node, 1);
        memory.tracks[1] = ReadTrack(mem_node, 2);
        memory.master = ReadMaster(mem_node);
        memory.loopFx = ReadLoopFx(mem_node);
        memory.rhythm = ReadRhythm(mem_node);
        memory.ctl = ReadCtl(GetNode(mem_node, "CTL"));

        for (int i = 1; i <= 8; ++i) {
            memory.assigns[i - 1] = ReadAssign(GetNode(mem_node, fmt::format("ASSIGN{}", i)));
        }

        ++idx;
    }

    return memory_file;
}

// ---------------------------------------------------------------------------
void MemoryFile::save(const std::filesystem::path& p) const
{
    pugi::xml_document doc;

    pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "utf-8";

    auto database = doc.append_child("database");
    database.append_attribute("name").set_value(database_name.c_str());
    database.append_attribute("revision").set_value(database_revision);

    for (int i = 0; i < memories.size(); ++i) {
        auto mem = database.append_child("mem");
        mem.append_attribute("id").set_value(i);
        mem.append_attribute("name").set_value(memories[i].name.to_string(i).c_str());

        SaveName(mem, memories[i].name, i);
        SaveTrack(mem, memories[i].tracks[0], 1);
        SaveTrack(mem, memories[i].tracks[1], 2);
        SaveMaster(mem, memories[i].master);
        SaveLoopFx(mem, memories[i].loopFx);
        SaveRhythm(mem, memories[i].rhythm);
        SaveCtl(mem, memories[i].ctl);

        for (int id = 0; id < memories[i].assigns.size(); ++id) {
            SaveAssign(mem, memories[i].assigns[id], id + 1);
        }
    }

    if (!doc.save_file(p.string().c_str(), "\t", pugi::xml_encoding::encoding_utf8)) {
        throw std::runtime_error("Error during writing " + p.string());
    }

    writeVersion(p.string(), database_counter);
}

}   // namespace BossRC
