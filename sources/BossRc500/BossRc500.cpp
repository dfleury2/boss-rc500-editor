#include "BossRc500.hpp"

#include <QStandardItemModel>

namespace {
// --------------------------------------------------------------------------
void
AddItemsToComboBox(QComboBox* cb, const std::vector<std::string>& list)
{
    int i = 0; // By default, UserDate (QVariant) is the same as item index
    for (auto& item: list) {
        cb->addItem(item.c_str(), i);
        ++i;
    }
}

// --------------------------------------------------------------------------
void
SetComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    if(!model) return;

    auto * item = model->item(index);
    if(!item) return;
    item->setEnabled(enabled);
}

// --------------------------------------------------------------------------
bool
IsComboBoxItemEnabled(QComboBox * comboBox, int index)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    if(!model) return false;

    auto * item = model->item(index);
    if(!item) return false;
    return item->isEnabled();
}

}


// --------------------------------------------------------------------------
namespace BossRc500 {

// --------------------------------------------------------------------------
void
SetMinMax(QComboBox* min, QComboBox* max, std::function<void(QComboBox*)> fct)
{
    fct(min);
    fct(max);
    max->setCurrentIndex(max->count() - 1);
}

// --------------------------------------------------------------------------
void NotAvailable(QComboBox* cb) { AddItemsToComboBox(cb, {"---"}); }

// --------------------------------------------------------------------------
void OffOn(QComboBox* cb) { AddItemsToComboBox(cb, {"Off", "On"}); }

// --------------------------------------------------------------------------
void
IntegerRange(QComboBox* cb, int min, int max)
{
    for (int i = min; i <= max; ++i) {
        cb->addItem(std::to_string(i).c_str());
    }
}

// --------------------------------------------------------------------------
void
DoubleRange(QComboBox* cb, double min, double max, double step)
{
    for (int i = min; i <= max; i += step) {
        cb->addItem(std::to_string(i).c_str());
    }
}

// --------------------------------------------------------------------------
void
Pan(QComboBox* cb)
{
    for (int i = 0; i <= 100; ++i) {
        std::string pan_label = "CENTER";
        if (i < 50) pan_label = "L" + std::to_string(50 - i);
        else if (i > 50) pan_label = "R" + std::to_string(i - 50);

        cb->addItem(pan_label.c_str());
    }
}

// --------------------------------------------------------------------------
void Start(QComboBox* cb) { AddItemsToComboBox(cb, {"IMMEDIATE", "FADE IN"}); }

// --------------------------------------------------------------------------
void Stop(QComboBox* cb) { AddItemsToComboBox(cb, {"IMMEDIATE", "FADE OUT", "LOOP END"}); }

// --------------------------------------------------------------------------
void
Measure(QComboBox* cb)
{
    cb->setIconSize(QSize{32, 32});
    cb->addItem("AUTO");
    cb->addItem("FREE");
    cb->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");
    cb->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
    cb->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
    cb->addItem(QIcon("./resources/images/quarter note dotted.png"), "Dot. Quarter Note");
    cb->addItem(QIcon("./resources/images/half note.png"), "Half Note");
    cb->addItem(QIcon("./resources/images/half note dotted.png"), "Dot. Half Note");
    for (int i = 1; i <= 16; ++i) {
        cb->addItem(std::to_string(i).c_str());
    }
}

// --------------------------------------------------------------------------
void Input(QComboBox* cb) { AddItemsToComboBox(cb, {"ALL", "MIC IN", "INST IN", "INST IN-A", "INST IN-B", "MIC/INST"}); }

// --------------------------------------------------------------------------
void Output(QComboBox* cb) { AddItemsToComboBox(cb, {"ALL", "OUT-A", "OUT-B"}); }

// --------------------------------------------------------------------------
void DubMode(QComboBox* cb) { AddItemsToComboBox(cb, {"OVERDUB", "REPLACE"}); }

// --------------------------------------------------------------------------
void RecAction(QComboBox* cb) { AddItemsToComboBox(cb, {"REC -> DUB", "REC -> PLAY"}); }

// --------------------------------------------------------------------------
void Quantize(QComboBox* cb) { AddItemsToComboBox(cb, {"OFF", "MEASURE"}); }

// --------------------------------------------------------------------------
void RecSource(QComboBox* cb) { AddItemsToComboBox(cb, {"ALL", "MIC IN", "INST", "INST-A", "INST-B"}); }

// --------------------------------------------------------------------------
void
LoopLength(QComboBox* cb)
{
    cb->addItem("AUTO");
    for (int i = 1; i <= 32; ++i) {
        cb->addItem(std::to_string(i).c_str());
    }
}

// --------------------------------------------------------------------------
void PlayMode(QComboBox* cb) { AddItemsToComboBox(cb, {"MULTI", "SINGLE"}); }

// --------------------------------------------------------------------------
void SingleChange(QComboBox* cb) { AddItemsToComboBox(cb, {"IMMEDIATE", "LOOP END"}); }

// --------------------------------------------------------------------------
void
FadeTime(QComboBox* cb)
{
    cb->setIconSize(QSize{32, 32});
    cb->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");
    cb->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
    cb->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
    cb->addItem(QIcon("./resources/images/half note.png"), "Half Note");
    for (int i = 1; i <= 32; ++i) {
        cb->addItem(std::to_string(i).c_str());
    }
}

// --------------------------------------------------------------------------
void AllStart(QComboBox* cb) { AddItemsToComboBox(cb, {"ALL", "TRACK1", "TRACK2"}); }

// --------------------------------------------------------------------------
void TrkChain(QComboBox* cb) { AddItemsToComboBox(cb, {"PARALLEL", "SERIES"}); }

// --------------------------------------------------------------------------
void TrkSelect(QComboBox* cb) { AddItemsToComboBox(cb, {"TRACK 1", "TRACK 2"}); }

// --------------------------------------------------------------------------
void
LoopFxType(QComboBox* cb)
{
    AddItemsToComboBox(cb, {
        "SCATTER-1", "SCATTER-2", "SCATTER-3", "SCATTER-4", // 0 - 3
        "REPEAT-1", "REPEAT-2", "REPEAT-3",                 // 4 - 6
        "SHIFT-1", "SHIFT-2",                               // 7 - 8
        "VINYL FLICK"                                       // 9
    });
}

bool IsScatter(int value) { return (value >= 0 && value <= 3); }
bool IsRepeat(int value) { return (value >= 4 && value <= 6); }
bool IsShift(int value) { return (value >= 7 && value <= 8); }
bool IsVinyl(int value) { return (value == 9); }

// --------------------------------------------------------------------------
void
ScatLen(QComboBox* cb)
{
    cb->setIconSize(QSize{32, 32});

    cb->addItem("THRU");
    cb->addItem(QIcon("./resources/images/whole note.png"), "Whole Note", 8);
    cb->addItem(QIcon("./resources/images/half note dotted.png"), "Half Note Dotted", 7);
    cb->addItem(QIcon("./resources/images/half note.png"), "Half Note", 6);
    cb->addItem(QIcon("./resources/images/quarter note dotted.png"), "Quarter Note Dotted", 5);
    cb->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note", 4);
    cb->addItem(QIcon("./resources/images/quaver.png"), "Quaver", 3);
    cb->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver", 2);
}

// --------------------------------------------------------------------------
void
ReptLen(QComboBox* cb)
{
    cb->setIconSize(QSize{32, 32});

    cb->addItem("THRU", 0);
    cb->addItem(QIcon("./resources/images/whole note.png"), "Whole Note", 8);
    cb->addItem(QIcon("./resources/images/half note dotted.png"), "Half Note Dotted", 7);
    cb->addItem(QIcon("./resources/images/half note.png"), "Half Note", 6);
    cb->addItem(QIcon("./resources/images/quarter note dotted.png"), "Quarter Note Dotted", 5);
    cb->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note", 4);
    cb->addItem(QIcon("./resources/images/quaver.png"), "Quaver", 3);
    cb->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver", 2);
    cb->addItem(QIcon("./resources/images/demi semi-quaver.png"), "Demi Semi-quaver", 1);
}

// --------------------------------------------------------------------------
void
Shift(QComboBox* cb)
{
    cb->setIconSize(QSize{32, 32});

    cb->addItem("THRU");
    cb->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver", 2);
    cb->addItem(QIcon("./resources/images/quaver.png"), "Quaver", 3);
    cb->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note", 4);
    cb->addItem(QIcon("./resources/images/quarter note dotted.png"), "Quarter Note Dotted", 5);
    cb->addItem(QIcon("./resources/images/half note.png"), "Half Note", 6);
    cb->addItem(QIcon("./resources/images/half note dotted.png"), "Half Note Dotted", 7);
    cb->addItem(QIcon("./resources/images/whole note.png"), "Whole Note", 8);
}

// --------------------------------------------------------------------------
void
ScatLen_EnableItems(QComboBox* cb, const Beat& beat)
{
    auto current_index = cb->currentIndex();

    SetComboBoxItemEnabled(cb, 0, true); // THRU - Always active
    SetComboBoxItemEnabled(cb, 1, false); // Whole Note
    SetComboBoxItemEnabled(cb, 2, false); // Half Note Dotted
    SetComboBoxItemEnabled(cb, 3, beat.x() == 2 || beat.x() == 4 || beat.x() == 8); // Half Note
    SetComboBoxItemEnabled(cb, 4, beat.y() == 8 && beat.x() % 3 == 0); // Quarter Note Dotted
    SetComboBoxItemEnabled(cb, 5, beat.y() == 4 || (beat.y() == 8 && (beat.x() == 8 || beat.x() == 10 || beat.x() == 14))); // Quarter Note
    SetComboBoxItemEnabled(cb, 6, true); // Quaver
    SetComboBoxItemEnabled(cb, 7, true); // Semi-quaver

    if (!IsComboBoxItemEnabled(cb, current_index)) {
        cb->setCurrentIndex(6); // Quaver as default
    }

}

// --------------------------------------------------------------------------
void
ReptLen_EnableItems(QComboBox* cb, const Beat& beat)
{
    auto current_index = cb->currentIndex();

    SetComboBoxItemEnabled(cb, 0, true); // THRU - Always active
    SetComboBoxItemEnabled(cb, 1, beat.x() == beat.y()); // Whole Note
    SetComboBoxItemEnabled(cb, 2, beat.x() % 3 == 0 && beat.x() != 9 && beat.x() != 15); // Half Note Dotted
    SetComboBoxItemEnabled(cb, 3, beat.x() * 2 == beat.y()); // Half Note
    SetComboBoxItemEnabled(cb, 4, beat.y() == 8 && beat.x() % 3 == 0 && beat.x() > beat.y()); // Quarter Note Dotted
    SetComboBoxItemEnabled(cb, 5, beat.y() == 4); // Quarter Note
    SetComboBoxItemEnabled(cb, 6, true); // Quaver
    SetComboBoxItemEnabled(cb, 7, true); // Semi-quaver
    SetComboBoxItemEnabled(cb, 8, true); // Demi Semi-quaver

    if (!IsComboBoxItemEnabled(cb, current_index)) {
        cb->setCurrentIndex(6); // Quaver as default
    }
}

// --------------------------------------------------------------------------
void
ShiftLen_EnableItems(QComboBox* cb, const Beat& beat)
{
    auto current_index = cb->currentIndex();

    SetComboBoxItemEnabled(cb, 0, true); // THRU - Always active
    SetComboBoxItemEnabled(cb, 1, true); // Semi-quaver
    SetComboBoxItemEnabled(cb, 2, true); // Quaver
    SetComboBoxItemEnabled(cb, 3, beat.y() == 4 || beat.x() == 8 || beat.x() == 10 || beat.x() == 14); // Quarter Note
    SetComboBoxItemEnabled(cb, 4, beat.y() == 8 && beat.x() % 3 == 0); // Quarter Note Dotted
    SetComboBoxItemEnabled(cb, 5, beat.x() == 2 || beat.x() == beat.y()); // Half Note
    SetComboBoxItemEnabled(cb, 6, beat.x() == 3 || beat.x() == 6 || beat.x() == 12 ); // Half Note Dotted
    SetComboBoxItemEnabled(cb, 7, beat.x() == beat.y()); // Whole Note

    if (!IsComboBoxItemEnabled(cb, current_index)) {
        cb->setCurrentIndex(2); // Quaver as default
    }
}


// --------------------------------------------------------------------------
void
RhythmPattern(QComboBox* cb)
{
    AddItemsToComboBox(cb, {
            "SimpleBeat1", "SimpleBeat2", "SimpleBeat3", "SimpleBeat4",
            "GrooveBeat1", "GrooveBeat2", "GrooveBeat3", "GrooveBeat4", "GrooveBeat5", "GrooveBeat6", "GrooveBeat7",
            "Rock1", "Rock2", "Rock3", "Rock4",
            "Funk1", "Funk2", "Funk3", "Funk4",
            "Shuffle1", "Shuffle2", "Shuffle3", "Shuffle4", "Shuffle5",
            "Swing1", "Swing2", "Swing3", "Swing4", "Swing5",
            "SideStick1", "SideStick2", "SideStick3", "SideStick4", "SideStick5",
            "PercusBeat1", "PercusBeat2", "PercusBeat3", "PercusBeat4",
            "LatinBeat1", "LatinBeat2", "LatinBeat3", "LatinBeat4",
            "Conga1", "Conga2", "Conga3",
            "Bossa1", "Bossa2",
            "Samba1", "Samba2",
            "DanceBeat1", "DanceBeat2", "DanceBeat3", "DanceBeat4",
            "Metronome1", "Metronome2", "Metronome3", "Metronome4",
            "Blank"
    });
}

// --------------------------------------------------------------------------
void RhythmVariation(QComboBox* cb) { AddItemsToComboBox(cb, {"A", "B"}); }

// --------------------------------------------------------------------------
void RhythmVarChange(QComboBox* cb) { AddItemsToComboBox(cb, {"MEASURE", "LOOP END"}); }

// --------------------------------------------------------------------------
void
RhythmKit(QComboBox* cb)
{
    AddItemsToComboBox(cb, {"Studio", "Live", "Light", "Heavy", "Rock", "Metal", "Jazz",
                            "Brush", "Cajon", "Drum & Bass", "Dance", "Techno", "Dance Beats",
                            "HipHop", "808+909"});
}

// --------------------------------------------------------------------------
void
RhythmBeat(QComboBox* cb)
{
    // 2/4, 3/4, 4/4, 5/4, 6/4, 7/4
    for (auto note_count = 2; note_count <= 7; ++note_count) { // X/4
        cb->addItem((std::to_string(note_count) + "/4").c_str(), QPoint{note_count, 4});
    }
    // 5/8, 6/8, 7/8, 8/8, 9/8, 10/8, 11/8,  12/8, 13/8, 14/8, 15/8
    for (auto note_count = 5; note_count <= 15; ++note_count) { // X/8
        cb->addItem((std::to_string(note_count) + "/8").c_str(), QPoint{note_count, 8});
    }
}

// --------------------------------------------------------------------------
void RhythmStart(QComboBox* cb) { AddItemsToComboBox(cb, {"LOOP START", "REC END", "BEFORE LOOP"}); }

// --------------------------------------------------------------------------
void RhythmStop(QComboBox* cb) { AddItemsToComboBox(cb, {"OFF", "LOOP STOP", "REC END"}); }

// --------------------------------------------------------------------------
void RhythmRecCount(QComboBox* cb) { AddItemsToComboBox(cb, {"OFF", "1-MEASURE"}); }

// --------------------------------------------------------------------------
void RhythmPlayCount(QComboBox* cb) { AddItemsToComboBox(cb, {"OFF", "1-MEASURE"}); }

// --------------------------------------------------------------------------
void
RhythmTone(QComboBox* cb)
{
    for (int i = -10; i <= 10; ++i) { // May be easier to understand if reversed here ?
        cb->addItem(((i > 0 ? "+" : "") + std::to_string(i)).c_str());
    }
}

// --------------------------------------------------------------------------
void
ControlPdlCtl(QComboBox* cb)
{
    AddItemsToComboBox(cb, {"OFF",
                            "T1 REC/PLY", "T1 R/P/S", "T1 R/P/S(CLR)", "T1 MON R/P", "T1 PLY/STP", "T1 P/S(CLR)", "T1 STOP", "T1 STOP(TAP)",
                            "T1 STOP(CLR)", "T1 STOP(T/C)", "T1 CLEAR", "T1 UND/RED", "T1 REVERSE",
                            "T2 REC/PLY", "T2 R/P/S", "T2 R/P/S(CLR)", "T2 MON R/P", "T2 PLY/STP", "T2 P/S(CLR)", "T2 STOP", "T2 STOP(TAP)",
                            "T2 STOP(CLR)", "T2 STOP(T/C)", "T2 CLEAR", "T2 UND/RED", "T2 REVERSE",
                            "TRK SELECT", "CUR REC/PLY", "CUR R/P/S", "CUR R/P/S(CLR)", "CUR MON R/P", "CUR PLY/STP", "CUR P/S(CLR)",
                            "CUR STOP", "CUR STP(TAP)", "CUR STP(CLR)",
                            "CUR STP(T/C)", "CUR CLEAR", "CUR UND/RED", "CUR REVERSE", "UNDO/REDO", "ALL START", "TAP TEMPO", "LOOP FX",
                            "TR1 FX", "TR2 FX", "CUR TR FX",
                            "FX INC", "FX DEC", "RHYTHM P/S", "RHYTHM PLAY", "RHYTHM STOP", "MEMORY INC", "MEMORY DEC", "MIC MUTE",
                            "EXTENT INC", "EXTENT DEC"
    });
}

// --------------------------------------------------------------------------
void
ControlExpr(QComboBox* cb)
{
    AddItemsToComboBox(cb, {"OFF",
                            "T1 LEVEL1", "T1 LEVEL2",
                            "T2 LEVEL1", "T2 LEVEL2",
                            "CUR LEVEL1", "CUR LEVEL2",
                            "TEMPO UP", "TEMPO DOWN",
                            "FX CONTROL",
                            "RHYTHM LEV1", "RHYTHM LEV2",
                            "MEMORY LEV1", "MEMORY LEV2"
    });
}

// --------------------------------------------------------------------------
void
AssignSource(QComboBox* cb)
{
    AddItemsToComboBox(cb, {
            "PEDAL1", "PEDAL2", "PEDAL3", "EXP PEDAL",
            "CTL1 PEDAL", "CTL2 PEDAL",
            "TR1 KNOB", "TR2 KNOB",
            "TR1 PLY/STOP", "TR2 PLY/STOP",
            "CUR TR CHG",
            "SYNC START",
            "CC 1", "CC 2", "CC 3", "CC 4", "CC 5", "CC 6", "CC 7", "CC 8", "CC 9",
            "CC 10", "CC 11", "CC 12", "CC 13", "CC 14", "CC 15", "CC 16", "CC 17", "CC 18", "CC 19",
            "CC 20", "CC 21", "CC 22", "CC 23", "CC 24", "CC 25", "CC 26", "CC 27", "CC 28", "CC 29",
            "CC 30", "CC 31",

            "CC 64", "CC 65", "CC 66", "CC 67", "CC 68", "CC 69",
            "CC 70", "CC 71", "CC 72", "CC 73", "CC 74", "CC 75", "CC 76", "CC 77", "CC 78", "CC 79",
            "CC 80", "CC 81", "CC 82", "CC 83", "CC 84", "CC 85", "CC 86", "CC 87", "CC 88", "CC 89",
            "CC 90", "CC 91", "CC 92", "CC 93", "CC 94", "CC 95"
    });
}

// --------------------------------------------------------------------------
void
AssignSourceMode(QComboBox* cb)
{
    AddItemsToComboBox(cb, {"MOMENT", "TOGGLE"});
}

// --------------------------------------------------------------------------
void
AssignTarget(QComboBox* cb)
{
    AddItemsToComboBox(cb, {
            "T1 REC/PLY", "T1 PLY/STP", "T1 CLEAR", "T1 UND/RED", "T1 REVERSE", "T1 1SHOT", "T1 LEVEL1", "T1 LEVEL2",
            "T1 PAN", "T1 START", "T1 STOP", "T1 LOOP.S", "T1 TEMPO.S", "T1 INPUT", "T1 OUTPUT",

            "T2 REC/PLY", "T2 PLY/STP", "T2 CLEAR", "T2 UND/RED", "T2 REVERSE", "T2 1SHOT", "T2 LEVEL1", "T2 LEVEL2",
            "T2 PAN", "T2 START", "T2 STOP", "T2 LOOP.S", "T2 TEMPO.S", "T2 INPUT", "T2 OUTPUT",

            "TRK SELECT", "CUR REC/PLY", "CUR PLY/STP", "CUR CLEAR", "CUR UND/RED", "CUR REVERSE", "CUR 1SHOT", "CUR LEVEL1", "CUR LEVEL2",
            "CUR PAN", "CUR START", "CUR STOP", "CUR LOOP.S", "CUR TEMPO.S", "CUR INPUT", "CUR OUTPUT",
            "UNDO/REDO", "ALL START", "TAP TEMPO", "TEMPO UP", "TEMPO DOWN", "TEMPO", "DUB MOD", "REC ACTION",
            "AUTO REC", "A. REC SENS", "A. REC SRC", "LOOP LENGTH", "PLAY MODE", "SINGL CHNGE", "FADE TIME",
            "ALL ST TRK", "TRK CHAIN", "LOOP FX", "TR1 FX", "TR2 FX", "CUR TR FX", "FX TYPE", "FX INC", "FX DEC",
            "FX CONTROL",
            "RHYTHM P/S", "RHYTHM PLAY", "RHYTHM STOP", "RHYTHM LEV1", "RHYTHM LEV2", "RHY REVERB",
            "RHY PATTERN", "VARIATION", "VAR. CHANGE", "KIT", "RHY START", "RHY STOP", "REC COUNT",
            "PLAY COUNT", "RHY FILL",
            "RHY PART1", "RHY PART2", "RHY PART3", "RHY PART4",
            "TONE LOW", "TONE HIGH",
            "MEMORY INC", "MEMORY DEC", "MEMORY LEV1", "MEMORY LEV2",
            "CC 1", "CC 2", "CC 3", "CC 4", "CC 5", "CC 6", "CC 7", "CC 8", "CC 9",
            "CC 10", "CC 11", "CC 12", "CC 13", "CC 14", "CC 15", "CC 16", "CC 17", "CC 18", "CC 19",
            "CC 20", "CC 21", "CC 22", "CC 23", "CC 24", "CC 25", "CC 26", "CC 27", "CC 28", "CC 29",
            "CC 30", "CC 31",

            "CC 64", "CC 65", "CC 66", "CC 67", "CC 68", "CC 69",
            "CC 70", "CC 71", "CC 72", "CC 73", "CC 74", "CC 75", "CC 76", "CC 77", "CC 78", "CC 79",
            "CC 80", "CC 81", "CC 82", "CC 83", "CC 84", "CC 85", "CC 86", "CC 87", "CC 88", "CC 89",
            "CC 90", "CC 91", "CC 92", "CC 93", "CC 94", "CC 95"
    });
}

// --------------------------------------------------------------------------
void AssignFxControl(QComboBox* cb, const Beat& beat, int loopFxType)
{
    if (IsScatter(loopFxType))
    {
        ScatLen(cb);
        ScatLen_EnableItems(cb, beat);
    }
    else if (IsRepeat(loopFxType)) {
        ReptLen(cb);
        ReptLen_EnableItems(cb,beat);
    }
    else if (IsShift(loopFxType)) {
        Shift(cb);
        ShiftLen_EnableItems(cb, beat);
    }
    else if (IsVinyl(loopFxType)) {
        IntegerRange(cb, 0, 100);
    }
}

}