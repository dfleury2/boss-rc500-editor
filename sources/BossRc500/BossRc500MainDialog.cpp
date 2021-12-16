#include "BossRc500MainDialog.hpp"
#include "BossRc500AssignDialog.hpp"

#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QInputDialog>
#include <QMenu>

#include <iostream>
#include <initializer_list>
#include <filesystem>

namespace {
// --------------------------------------------------------------------------
void AddItemsToComboBox(QComboBox* cb, std::initializer_list<const char*> list) {
    for (auto& item : list) {
        cb->addItem(item);
    }
}

}

// --------------------------------------------------------------------------
BossRc500MainDialog::BossRc500MainDialog(QDialog& dialog) :
        _parent(dialog)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    _parent.setFixedSize(_parent.width(), _parent.height());

    auto toolMenu = new QMenu(toolButton);
    toolMenu->addAction("New",          this, &BossRc500MainDialog::on_ToolMenu_New);
    toolMenu->addAction("Open...",      this, &BossRc500MainDialog::on_ToolMenu_Open);
    toolMenu->addSeparator();
    toolMenu->addAction("Save",         this, &BossRc500MainDialog::on_ToolMenu_Save);
    toolMenu->addAction("Save as...",   this, [this] { on_ToolMenu_Save(true); });
    toolMenu->addSeparator();
    toolMenu->addAction("Assign...",   this, &BossRc500MainDialog::on_ToolMenu_Assign);
    toolMenu->addSeparator();
    toolMenu->addAction("Quit",         [] { QApplication::exit(); });
    toolButton->setMenu(toolMenu);

    add_tooltips();
    add_combo_items();
    add_callbacks();

    // Start with a default MEMORY file
    on_ToolMenu_New();
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::add_tooltips()
{
// Add tooltips
#if QT_CONFIG(tooltip)
    track1_Reverse->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_reverse.png\"/></p></body></html>", nullptr));
    track1_LoopFx->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_loopfx.png\"/></p></body></html>", nullptr));
    track1_OneShot->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_oneshot.png\"/></p></body></html>", nullptr));
    track1_Pan->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_pan.png\"/></p></body></html>", nullptr));
    track1_Start->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_start.png\"/></p></body></html>", nullptr));
    track1_Stop->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_stop.png\"/></p></body></html>", nullptr));
    track1_Measure->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_measure.png\"/></p></body></html>", nullptr));
    track1_LoopSync->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_loopsync.png\"/></p></body></html>", nullptr));
    track1_TempoSync->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_temposync.png\"/></p></body></html>", nullptr));
    track1_Input->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_input.png\"/></p></body></html>", nullptr));
    track1_Output->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_output.png\"/></p></body></html>", nullptr));

    track2_Level->setToolTip(std::to_string(track2_Level->value()).c_str());
    track2_Reverse->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_reverse.png\"/></p></body></html>", nullptr));
    track2_LoopFx->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_loopfx.png\"/></p></body></html>", nullptr));
    track2_OneShot->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_oneshot.png\"/></p></body></html>", nullptr));
    track2_Pan->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_pan.png\"/></p></body></html>", nullptr));
    track2_Start->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_start.png\"/></p></body></html>", nullptr));
    track2_Stop->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_stop.png\"/></p></body></html>", nullptr));
    track2_Measure->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_measure.png\"/></p></body></html>", nullptr));
    track2_LoopSync->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_loopsync.png\"/></p></body></html>", nullptr));
    track2_TempoSync->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_temposync.png\"/></p></body></html>", nullptr));
    track2_Input->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_input.png\"/></p></body></html>", nullptr));
    track2_Output->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/track_output.png\"/></p></body></html>", nullptr));

    record_DubMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_rec_dubmode.png\"/></p></body></html>", nullptr));
    record_RecordAction->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_rec_recaction.png\"/></p></body></html>", nullptr));
    record_Quantize->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_rec_quantize.png\"/></p></body></html>", nullptr));
    record_AutoRecord->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_rec_autorec.png\"/></p></body></html>", nullptr));
    record_AutoRecordSensitivity->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_rec_autorecsens.png\"/></p></body></html>", nullptr));
    record_AutoRecordSource->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_rec_autorecsrc.png\"/></p></body></html>", nullptr));
    record_LoopLength->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_rec_looplength.png\"/></p></body></html>", nullptr));

    play_PlayMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_play_playmode.png\"/></p></body></html>", nullptr));
    play_SingleChange->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_play_singlechange.png\"/></p></body></html>", nullptr));
    play_Level->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_play_level.png\"/></p></body></html>", nullptr));
    play_FadeTime->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_play_fadetime.png\"/></p></body></html>", nullptr));
    play_AllStart->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_play_allstart.png\"/></p></body></html>", nullptr));
    play_TrackChain->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/master_play_trackchain.png\"/></p></body></html>", nullptr));

    loopFx_Sw->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/loopfx_loopfx.png\"/></p></body></html>", nullptr));
    loopFx_Type->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/loopfx_type.png\"/></p></body></html>", nullptr));
    loopFx_ScatLen->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/loopfx_scatlen.png\"/></p></body></html>", nullptr));
    loopFx_ReptLen->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/loopfx_replen.png\"/></p></body></html>", nullptr));
    loopFx_Shift->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/loopfx_shift.png\"/></p></body></html>", nullptr));
    loopFx_Flick->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/loopfx_flick.png\"/></p></body></html>", nullptr));

    rhythm_Level->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_level.png\"/></p></body></html>", nullptr));
    rhythm_Reverb->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_reverb.png\"/></p></body></html>", nullptr));
    rhythm_Pattern->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_pattern.png\"/></p></body></html>", nullptr));
    rhythm_Variation->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_variation.png\"/></p></body></html>", nullptr));
    rhythm_VarChange->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_varchange.png\"/></p></body></html>", nullptr));
    rhythm_Kit->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_kit.png\"/></p></body></html>", nullptr));
    rhythm_Beat->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_beat.png\"/></p></body></html>", nullptr));
    rhythm_Start->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_start.png\"/></p></body></html>", nullptr));
    rhythm_Stop->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_stop.png\"/></p></body></html>", nullptr));
    rhythm_RecCount->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_reccount.png\"/></p></body></html>", nullptr));
    rhythm_PlayCount->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_playcount.png\"/></p></body></html>", nullptr));
    rhythm_Fill->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_fill.png\"/></p></body></html>", nullptr));
    rhythm_Part1->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_part1-4.png\"/></p></body></html>", nullptr));
    rhythm_Part2->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_part1-4.png\"/></p></body></html>", nullptr));
    rhythm_Part3->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_part1-4.png\"/></p></body></html>", nullptr));
    rhythm_Part4->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_part1-4.png\"/></p></body></html>", nullptr));
    rhythm_ToneLow->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_tonelow.png\"/></p></body></html>", nullptr));
    rhythm_ToneHigh->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/rhythm_tonehigh.png\"/></p></body></html>", nullptr));

    control_Pedal1->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/control_pdlctl.png\"/></p></body></html>", nullptr));
    control_Pedal2->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/control_pdlctl.png\"/></p></body></html>", nullptr));
    control_Pedal3->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/control_pdlctl.png\"/></p></body></html>", nullptr));
    control_Control1->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/control_pdlctl.png\"/></p></body></html>", nullptr));
    control_Control2->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/control_pdlctl.png\"/></p></body></html>", nullptr));
    control_Expression->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/control_exp.png\"/></p></body></html>", nullptr));

#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::add_combo_items()
{
    // MEMORY
    for (int i = 1; i <= 99; ++i) {
        cb_Memory->addItem(std::to_string(i).c_str());
        cb_CopyFrom->addItem(std::to_string(i).c_str());
        cb_CopyTo->addItem(std::to_string(i).c_str());
    }

    // ----- TRACK 1/2 -----
    // PAN
    auto pan_items = [](QComboBox* cb) {
        for (int i = 0; i <= 100; ++i) {
            std::string pan_label = "CENTER";
            if (i < 50) pan_label = "L" + std::to_string(50 - i);
            else if (i > 50) pan_label = "R" + std::to_string(i - 50);

            cb->addItem(pan_label.c_str());
        }
    };
    pan_items(track1_Pan);
    pan_items(track2_Pan);

    // START
    auto start_items = [](QComboBox* cb) {
        AddItemsToComboBox(cb, {"IMMEDIATE", "FADE IN"});
    };
    start_items(track1_Start);
    start_items(track2_Start);

    // STOP
    auto stop_items = [](QComboBox* cb) {
        AddItemsToComboBox(cb, {"IMMEDIATE", "FADE OUT", "LOOP END"});
    };
    stop_items(track1_Stop);
    stop_items(track2_Stop);

    // MEASURE
    auto measure_items = [](QComboBox* cb) {
        cb->addItem("AUTO");
        cb->addItem("FREE");
        cb->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");
        cb->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
        cb->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
        cb->addItem(QIcon("./resources/images/half note.png"), "Half Note");
        for (int i = 1; i <= 16; ++i) {
            cb->addItem(std::to_string(i).c_str());
        }
    };
    measure_items(track1_Measure);
    measure_items(track2_Measure);

    // INPUT
    auto input_items = [](QComboBox* cb) {
        AddItemsToComboBox(cb, {"ALL", "MIC IN", "INST IN", "INST IN-A", "INST IN-B", "MIC/INST"});
    };
    input_items(track1_Input);
    input_items(track2_Input);

    // OUTPUT
    auto output_items = [](QComboBox* cb) {
        AddItemsToComboBox(cb, {"ALL", "OUT-A", "OUT-B"});
    };
    output_items(track1_Output);
    output_items(track2_Output);

    // ----- RECORD -----
    AddItemsToComboBox(record_DubMode, {"OVERDUB", "REPLACE"});
    AddItemsToComboBox(record_RecordAction, {"REC -> DUB", "REC -> PLAY"});
    AddItemsToComboBox(record_Quantize, {"OFF", "MEASURE"});
    AddItemsToComboBox(record_AutoRecordSource, {"ALL", "MIC IN", "INST", "INST-A", "INST-B"});

    record_LoopLength->addItem("AUTO");
    for (int i = 1; i <= 32; ++i)
        record_LoopLength->addItem(std::to_string(i).c_str());

    // ----- PLAY -----
    AddItemsToComboBox(play_PlayMode, {"MULTI", "SINGLE"});
    AddItemsToComboBox(play_SingleChange, {"IMMEDIATE", "LOOP END"});

    play_FadeTime->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");
    play_FadeTime->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
    play_FadeTime->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
    play_FadeTime->addItem(QIcon("./resources/images/half note.png"), "Half Note");
    for (int i = 1; i <= 32; ++i) {
        play_FadeTime->addItem(std::to_string(i).c_str());
    }

    AddItemsToComboBox(play_AllStart, {"ALL", "TRACK1", "TRACK2"});
    AddItemsToComboBox(play_TrackChain, {"PARALLEL", "SERIES"});

    // ----- LOOP FX -----
    AddItemsToComboBox(loopFx_Type, {"SCATTER-1", "SCATTER-2","SCATTER-3", "SCATTER-4",
                                     "REPEAT-1", "REPEAT-2","REPEAT-3", "SHIFT-1", "SHIFT-2", "VINYL FLICK"});

    loopFx_ScatLen->addItem("THRU");
    loopFx_ScatLen->addItem(QIcon("./resources/images/half note.png"), "Half Note");
    loopFx_ScatLen->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
    loopFx_ScatLen->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
    loopFx_ScatLen->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");

    loopFx_ReptLen->addItem("THRU");
    loopFx_ReptLen->addItem(QIcon("./resources/images/whole note.png"), "Whole Note");
    loopFx_ReptLen->addItem(QIcon("./resources/images/half note.png"), "Half Note");
    loopFx_ReptLen->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
    loopFx_ReptLen->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
    loopFx_ReptLen->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");
    loopFx_ReptLen->addItem(QIcon("./resources/images/demi semi-quaver.png"), "Demi Semi-quaver");

    loopFx_Shift->addItem("THRU");
    loopFx_Shift->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");
    loopFx_Shift->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
    loopFx_Shift->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
    loopFx_Shift->addItem(QIcon("./resources/images/half note.png"), "Half Note");
    loopFx_Shift->addItem(QIcon("./resources/images/whole note.png"), "Whole Note");

    // ----- RHYTHM ----
    AddItemsToComboBox(rhythm_Pattern, {
        "SimpleBeat1", "SimpleBeat2", "SimpleBeat3", "SimpleBeat4",
        "GrooveBeat1", "GrooveBeat2", "GrooveBeat3", "GrooveBeat4", "GrooveBeat5", "GrooveBeat6", "GrooveBeat7",
        "Rock1", "Rock2", "Rock3", "Rock4",
        "Funk1", "Funk2", "Funk3", "Funk4",
        "Shuffle1", "Shuffle2", "Shuffle3", "Shuffle4", "Shuffle5",
        "Swing1", "Swing2", "Swing3", "Swing4", "Swing5",
        "Sidekick1", "Sidekick2", "Sidekick3", "Sidekick4", "Sidekick5",
        "PercusBeat1", "PercusBeat2", "PercusBeat3", "PercusBeat4",
        "LatinBeat1", "LatinBeat2", "LatinBeat3", "LatinBeat4",
        "Conga1", "Conga2", "Conga3",
        "Bossa1", "Bossa2",
        "Samba1", "Samba2",
        "DanceBeat1", "DanceBeat2", "DanceBeat3", "DanceBeat4",
        "Metronome1", "Metronome2", "Metronome3", "Metronome4",
        "Blank"
    });
    AddItemsToComboBox(rhythm_Variation, {"A", "B"});
    AddItemsToComboBox(rhythm_VarChange, {"MEASURE", "LOOP END"});
    AddItemsToComboBox(rhythm_Kit, {"Studio", "Live", "Light", "Heavy", "Rock", "Metal", "Jazz",
                                    "Brush", "Cajon", "Drum & Bass", "Dance", "Techno", "Dance Beats",
                                    "HipHop", "808+909"});
    AddItemsToComboBox(rhythm_Beat, {"2/4", "3/4", "4/4", "5/4", "6/4", "7/4",
                                     "5/8", "6/8", "7/8", "8/8", "9/8", "10/8", "11/8", "12/8", "13/8", "14/8", "15/8"});
    AddItemsToComboBox(rhythm_Start, {"LOOP START", "REC END", "BEFORE LOOP"});
    AddItemsToComboBox(rhythm_Stop, {"OFF", "LOOP STOP", "REC END"});
    AddItemsToComboBox(rhythm_RecCount, {"OFF", "1-MEASURE"});
    AddItemsToComboBox(rhythm_PlayCount, {"OFF", "1-MEASURE"});

    // ----- CONTROL -----
    auto ctl_items = {"OFF",
        "T1 REC/PLY", "T1 R/P/S", "T1 R/P/S(CLR)", "T1 MON R/P", "T1 PLY/STP", "T1 P/S(CLR)", "T1 STOP", "T1 STOP(TAP)", "T1 STOP(CLR)", "T1 STOP(T/C)", "T1 CLEAR", "T1 UND/RED", "T1 REVERSE",
        "T2 REC/PLY", "T2 R/P/S", "T2 R/P/S(CLR)", "T2 MON R/P", "T2 PLY/STP", "T2 P/S(CLR)", "T2 STOP", "T2 STOP(TAP)", "T2 STOP(CLR)", "T2 STOP(T/C)", "T2 CLEAR", "T2 UND/RED", "T2 REVERSE",
        "TRK SELECT", "CUR REC/PLY", "CUR R/P/S", "CUR R/P/S(CLR)", "CUR MON R/P", "CUR PLY/STP", "CUR P/S(CLR)", "CUR STOP", "CUR STP(TAP)", "CUR STP(CLR)",
        "CUR STP(T/C)", "CUR CLEAR", "CUR UND/RED", "CUR REVERSE", "UNDO/REDO", "ALL START", "TAP TEMPO", "LOOP FX", "TR1 FX", "TR2 FX", "CUR TR FX",
        "FX INC", "FX DEC", "RHYTHM P/S", "RHYTHM PLAY", "RHYTHM STOP", "MEMORY INC", "MEMORY DEC", "MIC MUTE", "EXTENT INC", "EXTENT DEC"
        };
    AddItemsToComboBox(control_Pedal1, ctl_items);
    AddItemsToComboBox(control_Pedal2, ctl_items);
    AddItemsToComboBox(control_Pedal3, ctl_items);
    AddItemsToComboBox(control_Control1, ctl_items);
    AddItemsToComboBox(control_Control2, ctl_items);

    AddItemsToComboBox(control_Expression,
            {"OFF",
             "T1 LEVEL1", "T1 LEVEL2",
             "T2 LEVEL1", "T2 LEVEL2",
             "CUR LEVEL1", "CUR LEVEL2",
             "TEMPO UP", "TEMPO DOWN",
             "FX CONTROL",
             "RHYTHM LEV1", "RHYTHM LEV2",
             "MEMORY LEV1", "MEMORY LEV2", });
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Copy, &QPushButton::pressed, this, &BossRc500MainDialog::on_copy);

    QObject::connect(cb_Memory, &QComboBox::currentIndexChanged, this, &BossRc500MainDialog::on_memory_changed);
    QObject::connect(button_Edit, &QPushButton::pressed, this, &BossRc500MainDialog::on_edit);

    // Track 1/2 callbacks
    QObject::connect(track1_Level, &QSlider::valueChanged, this, [this] { on_Level_changed(track1_Level); });
    QObject::connect(track2_Level, &QSlider::valueChanged, this, [this] { on_Level_changed(track2_Level); });

    QObject::connect(track1_Reverse, &QCheckBox::stateChanged, this, [this] { on_Reverse_changed(track1_Reverse); });
    QObject::connect(track2_Reverse, &QCheckBox::stateChanged, this, [this] { on_Reverse_changed(track2_Reverse); });

    QObject::connect(track1_LoopFx, &QCheckBox::stateChanged, this, [this] { on_LoopFx_changed(track1_LoopFx); });
    QObject::connect(track2_LoopFx, &QCheckBox::stateChanged, this, [this] { on_LoopFx_changed(track2_LoopFx); });

    QObject::connect(track1_OneShot, &QCheckBox::stateChanged, this, [this] { on_OneShot_changed(track1_OneShot); });
    QObject::connect(track2_OneShot, &QCheckBox::stateChanged, this, [this] { on_OneShot_changed(track2_OneShot); });

    QObject::connect(track1_Pan, &QComboBox::currentIndexChanged, this, [this] { on_Pan_changed(track1_Pan); });
    QObject::connect(track2_Pan, &QComboBox::currentIndexChanged, this, [this] { on_Pan_changed(track2_Pan); });

    QObject::connect(track1_Start, &QComboBox::currentIndexChanged, this, [this] { on_Start_changed(track1_Start); });
    QObject::connect(track2_Start, &QComboBox::currentIndexChanged, this, [this] { on_Start_changed(track2_Start); });

    QObject::connect(track1_Stop, &QComboBox::currentIndexChanged, this, [this] { on_Stop_changed(track1_Stop); });
    QObject::connect(track2_Stop, &QComboBox::currentIndexChanged, this, [this] { on_Stop_changed(track2_Stop); });

    QObject::connect(track1_Measure, &QComboBox::currentIndexChanged, this, [this] { on_Measure_changed(track1_Measure); });
    QObject::connect(track2_Measure, &QComboBox::currentIndexChanged, this, [this] { on_Measure_changed(track2_Measure); });

    QObject::connect(track1_LoopSync, &QCheckBox::stateChanged, this, [this] { on_LoopSync_changed(track1_LoopSync); });
    QObject::connect(track2_LoopSync, &QCheckBox::stateChanged, this, [this] { on_LoopSync_changed(track2_LoopSync); });

    QObject::connect(track1_TempoSync, &QCheckBox::stateChanged, this, [this] { on_TempoSync_changed(track1_TempoSync); });
    QObject::connect(track2_TempoSync, &QCheckBox::stateChanged, this, [this] { on_TempoSync_changed(track2_TempoSync); });

    QObject::connect(track1_Input, &QComboBox::currentIndexChanged, this, [this] { on_Input_changed(track1_Input); });
    QObject::connect(track2_Input, &QComboBox::currentIndexChanged, this, [this] { on_Input_changed(track2_Input); });

    QObject::connect(track1_Output, &QComboBox::currentIndexChanged, this, [this] { on_Output_changed(track1_Output); });
    QObject::connect(track2_Output, &QComboBox::currentIndexChanged, this, [this] { on_Output_changed(track2_Output); });

    // Master Record callbacks
    QObject::connect(record_DubMode, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(record_DubMode, "DubMode"); });

    QObject::connect(record_RecordAction, &QComboBox::currentIndexChanged, this,
            [this] { on_Master_ComboBox_changed(record_RecordAction, "RecAction"); });

    QObject::connect(record_Quantize, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(record_Quantize, "RecQuantize"); });

    QObject::connect(record_AutoRecord, &QCheckBox::stateChanged, this,
            [this] { on_Master_CheckBox_changed(record_AutoRecord, "AutoRec"); });

    QObject::connect(record_AutoRecordSensitivity, &QSpinBox::valueChanged,
            this, [this] { on_Master_SpinBox_changed(record_AutoRecordSensitivity, "AutoRecSens"); });

    QObject::connect(record_AutoRecordSource, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(record_AutoRecordSource, "AutoRecSrc"); });

    QObject::connect(record_LoopLength, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(record_LoopLength, "LpLen"); });

    // Master Play callbacks
    QObject::connect(play_Tempo, &QDoubleSpinBox::valueChanged,
            this, [this] { on_Master_DoubleSpinBox_changed(play_Tempo, "Tempo", 10); });

    QObject::connect(play_PlayMode, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(play_PlayMode, "PlayMode"); });

    QObject::connect(play_SingleChange, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(play_SingleChange, "SinglPlayeChange"); });

    QObject::connect(play_Level, &QSpinBox::valueChanged,
            this, [this] { on_Master_SpinBox_changed(play_Level, "Level"); });

    QObject::connect(play_FadeTime, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(play_FadeTime, "FadeTime"); });

    QObject::connect(play_AllStart, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(play_AllStart, "AllStart"); });

    QObject::connect(play_TrackChain, &QComboBox::currentIndexChanged,
            this, [this] { on_Master_ComboBox_changed(play_TrackChain, "TrackChain"); });

    // LoopFX callbacks
    QObject::connect(loopFx_Sw, &QCheckBox::stateChanged, this,
            [this] { on_LoopFx_CheckBox_changed(loopFx_Sw, "Sw"); });

    QObject::connect(loopFx_Type, &QComboBox::currentIndexChanged,
            this, [this] { on_LoopFx_ComboBox_changed(loopFx_Type, "FxType"); });

    QObject::connect(loopFx_ScatLen, &QComboBox::currentIndexChanged,
            this, [this] { on_LoopFx_ComboBox_changed(loopFx_ScatLen, "ScatterLength"); });

    QObject::connect(loopFx_ReptLen, &QComboBox::currentIndexChanged,
            this, [this] { on_LoopFx_ComboBox_changed(loopFx_ReptLen, "RepeatLength"); });

    QObject::connect(loopFx_Shift, &QComboBox::currentIndexChanged,
            this, [this] { on_LoopFx_ComboBox_changed(loopFx_Shift, "ShiftShift"); });

    QObject::connect(loopFx_Flick, &QSpinBox::valueChanged,
            this, [this] { on_LoopFx_SpinBox_changed(loopFx_Flick, "VinylFlickFlick"); });

    // Rhythm callback
    QObject::connect(rhythm_Level, &QSlider::valueChanged,
            this, [this] { on_Rhythm_Slider_changed(rhythm_Level, "Level"); });

    QObject::connect(rhythm_Reverb, &QSlider::valueChanged,
            this, [this] { on_Rhythm_Slider_changed(rhythm_Reverb, "Reverb"); });

    QObject::connect(rhythm_Pattern, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_Pattern, "Pattern"); });

    QObject::connect(rhythm_Variation, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_Variation, "Variation"); });

    QObject::connect(rhythm_VarChange, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_VarChange, "VariationChange"); });

    QObject::connect(rhythm_Kit, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_Kit, "Kit"); });

    QObject::connect(rhythm_Beat, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_Beat, "Beat"); });

    QObject::connect(rhythm_Start, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_Start, "Start"); });

    QObject::connect(rhythm_Stop, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_Stop, "Stop"); });

    QObject::connect(rhythm_RecCount, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_RecCount, "RecCount"); });

    QObject::connect(rhythm_PlayCount, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_PlayCount, "PlayCount"); });

    QObject::connect(rhythm_Fill, &QCheckBox::stateChanged, this,
            [this] { on_Rhythm_CheckBox_changed(rhythm_Fill, "Fill"); });

    QObject::connect(rhythm_Part1, &QCheckBox::stateChanged, this,
            [this] { on_Rhythm_CheckBox_changed(rhythm_Part1, "Part1"); });
    QObject::connect(rhythm_Part2, &QCheckBox::stateChanged, this,
            [this] { on_Rhythm_CheckBox_changed(rhythm_Part2, "Part2"); });
    QObject::connect(rhythm_Part3, &QCheckBox::stateChanged, this,
            [this] { on_Rhythm_CheckBox_changed(rhythm_Part3, "Part3"); });
    QObject::connect(rhythm_Part4, &QCheckBox::stateChanged, this,
            [this] { on_Rhythm_CheckBox_changed(rhythm_Part4, "Part4"); });

    QObject::connect(rhythm_ToneLow, &QSlider::valueChanged,
            this, [this] { on_Rhythm_Slider_changed(rhythm_ToneLow, "ToneLow"); });
    QObject::connect(rhythm_ToneHigh, &QSlider::valueChanged,
            this, [this] { on_Rhythm_Slider_changed(rhythm_ToneHigh, "ToneHigh"); });

    // Control callbacks
    QObject::connect(control_Pedal1, &QComboBox::currentIndexChanged,
            this, [this] { on_Control_ComboBox_changed(control_Pedal1, "Pedal1"); });
    QObject::connect(control_Pedal2, &QComboBox::currentIndexChanged,
            this, [this] { on_Control_ComboBox_changed(control_Pedal2, "Pedal2"); });
    QObject::connect(control_Pedal3, &QComboBox::currentIndexChanged,
            this, [this] { on_Control_ComboBox_changed(control_Pedal3, "Pedal3"); });

    QObject::connect(control_Control1, &QComboBox::currentIndexChanged,
            this, [this] { on_Control_ComboBox_changed(control_Control1, "Ctl1"); });
    QObject::connect(control_Control2, &QComboBox::currentIndexChanged,
            this, [this] { on_Control_ComboBox_changed(control_Control2, "Ctl2"); });

    QObject::connect(control_Expression, &QComboBox::currentIndexChanged,
            this, [this] { on_Control_ComboBox_changed(control_Expression, "Exp"); });
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_New()
{
    try {
        setFilename("");
        load_database("./resources/templates/MEMORY_DEFAULT.RC0");

        cb_Memory->setCurrentIndex(0);
        load_memory(cb_Memory->currentIndex());
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_Open()
{
    try {
        auto filename = QFileDialog::getOpenFileName(&_parent,
                tr("Open a MEMORY file"), "", tr("Memory Files (*.RC0)")).toStdString();
        if (!filename.empty()) {
            setFilename(filename);
            load_database(_filename);

            cb_Memory->setCurrentIndex(0);
            load_memory(cb_Memory->currentIndex());
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
        setFilename("");
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_Save(bool askFilename)
{
    try {
        if (_filename.empty() || askFilename) {
            auto filename = QFileDialog::getOpenFileName(&_parent,
                    tr("Save to MEMORY file"), "", tr("Memory Files (*.RC0)")).toStdString();
            if (filename.empty()) {
                return;
            }

            setFilename(filename);
        }

        if (auto response = QMessageBox::question(nullptr,
                    "Write changes to file ?",
                    QString::fromStdString("Do you want to write to file:\n" + _filename));
                response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        WriteMemoryDatabase(_database, _filename);

        std::filesystem::path path = _filename;
        bool memory1_detected = (path.filename() == "MEMORY1.RC0");
        if (memory1_detected) {
            path.replace_filename("MEMORY2.RC0");
            WriteMemoryDatabase(_database, path.string());
        }

        std::string message = "Database successfully written to file.";
        if (memory1_detected) {
            message += "\nNote: MEMORY2.RC0 file has been written too.";
        }
        QMessageBox(QMessageBox::Information, "Information", message.c_str()).exec();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_Assign()
{
    try {
        QDialog dialog;
        BossRc500AssignDialog assignDialog(dialog, _database, cb_Memory->currentIndex());
        dialog.setWindowTitle("BOSS RC-500 - Assign");
        dialog.setModal(true);
        dialog.exec();
        if (assignDialog.apply) {
            _database = std::move(assignDialog.database);
        }

    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_copy()
{
    try {
        int memory_slot = std::stoi(cb_Memory->currentText().toStdString());
        int copy_from_slot = std::stoi(cb_CopyFrom->currentText().toStdString());
        int copy_to_slot = std::stoi(cb_CopyTo->currentText().toStdString());

        if (copy_from_slot > copy_to_slot) {
            throw std::runtime_error("Copy From slot must be less than copy to slot");
        }

        if (auto response = QMessageBox::question(nullptr,
                "Copy memories ?",
                QString::fromStdString("Do you want to copy memory slot " + std::to_string(memory_slot) +
                "\ninto slots from " + std::to_string(copy_from_slot) + " to " +
                std::to_string(copy_to_slot))); response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        // Update database and write it to disk
        const auto& slot = _database["mem"][memory_slot - 1];

        for (int i = copy_from_slot; i <= copy_to_slot; ++i) {
            _database["mem"][i - 1] = slot;
            // Restore the memory id of the copied slot
            _database["mem"][i - 1]["id"] = i - 1;
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}


// --------------------------------------------------------------------------
void BossRc500MainDialog::on_edit()
{
    try {
        int memory_index = cb_Memory->currentIndex();

        bool ok = false;
        auto text = QInputDialog::getText(nullptr, "Memory Name",
                "Memory Name", QLineEdit::Normal,
                _database["mem"][memory_index]["name"].get<std::string>().c_str(), &ok).toStdString();
        if (ok) {
            _database["mem"][memory_index]["name"] = text;

            // Update NAME.C01 to NAME.C12
            for (int i = 0; i < 12; ++i) {
                std::string cxx = "C";
                cxx += (i + 1 < 10 ? "0" : "") + std::to_string(i + 1);

                int value = (i < text.size() ? text[i] : 32);
                _database["mem"][memory_index]["NAME"][cxx] = (value >= 32 && value <= 127 ? value : '_');
            }

            auto name = std::to_string(memory_index + 1);
            if (!text.empty()) {
                name += " - " + text;
            }
            cb_Memory->setItemText(memory_index, name.c_str());
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainDialog::on_memory_changed()
{
    load_memory(cb_Memory->currentIndex());
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::load_database(const std::string& filename)
{
    _database = ReadMemoryDatabase(filename);

    // Add name to memory index
    for (int i = 1; i <= 99; ++i) {
        auto index = std::to_string(i);
        if (auto found_name = _database["mem"][i - 1].find("name");
                found_name != _database["mem"][i - 1].end()) {
            index += " - " + found_name->get<std::string>();
        }
        std::cout << "index: " << index << std::endl;
        cb_Memory->setItemText(i - 1, index.c_str());
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::load_memory(int memory_index)
{
    { // TRACK 1
        auto& track1 = _database["mem"][memory_index]["TRACK"][0];

        track1_Reverse->setChecked(track1["Rev"].get<int>());
        track1_Level->setValue(track1["PlyLvl"].get<int>());
        track1_Pan->setCurrentIndex(track1["Pan"].get<int>());
        track1_OneShot->setChecked(track1["One"].get<int>());
        track1_LoopFx->setChecked(track1["LoopFx"].get<int>());
        track1_Start->setCurrentIndex(track1["StrtMod"].get<int>());
        track1_Stop->setCurrentIndex(track1["StpMod"].get<int>());
        track1_Measure->setCurrentIndex(track1["Measure"].get<int>());
        track1_LoopSync->setChecked(track1["LoopSync"].get<int>());
        track1_TempoSync->setChecked(track1["TempoSync"].get<int>());
        track1_Input->setCurrentIndex(track1["Input"].get<int>());
        track1_Output->setCurrentIndex(track1["Output"].get<int>());
    }

    { // TRACK 2
        auto& track2 = _database["mem"][memory_index]["TRACK"][1];

        track2_Reverse->setChecked(track2["Rev"].get<int>());
        track2_Level->setValue(track2["PlyLvl"].get<int>());
        track2_Pan->setCurrentIndex(track2["Pan"].get<int>());
        track2_OneShot->setChecked(track2["One"].get<int>());
        track2_LoopFx->setChecked(track2["LoopFx"].get<int>());
        track2_Start->setCurrentIndex(track2["StrtMod"].get<int>());
        track2_Stop->setCurrentIndex(track2["StpMod"].get<int>());
        track2_Measure->setCurrentIndex(track2["Measure"].get<int>());
        track2_LoopSync->setChecked(track2["LoopSync"].get<int>());
        track2_TempoSync->setChecked(track2["TempoSync"].get<int>());
        track2_Input->setCurrentIndex(track2["Input"].get<int>());
        track2_Output->setCurrentIndex(track2["Output"].get<int>());
    }

    // MASTER (REC/PLAY)
    {
        auto& master = _database["mem"][memory_index]["MASTER"];
        record_DubMode->setCurrentIndex(master["DubMode"].get<int>());

        // RECORD
        record_DubMode->setCurrentIndex(master["DubMode"].get<int>());
        record_RecordAction->setCurrentIndex(master["RecAction"].get<int>());
        record_Quantize->setCurrentIndex(master["RecQuantize"].get<int>());
        record_AutoRecord->setChecked(master["AutoRec"].get<int>());
        record_AutoRecordSensitivity->setValue(master["AutoRecSens"].get<int>());
        record_AutoRecordSource->setCurrentIndex(master["AutoRecSrc"].get<int>());
        record_LoopLength->setCurrentIndex(master["LpLen"].get<int>());

        // PLAY
        play_Tempo->setValue(master["Tempo"].get<int>() / 10.0);
        play_PlayMode->setCurrentIndex(master["PlayMode"].get<int>());
        play_SingleChange->setCurrentIndex(master["SinglPlayeChange"].get<int>());
        play_Level->setValue(master["Level"].get<int>());
        play_FadeTime->setCurrentIndex(master["FadeTime"].get<int>());
        play_AllStart->setCurrentIndex(master["AllStart"].get<int>());
        play_TrackChain->setCurrentIndex(master["TrackChain"].get<int>());
    }

    // LOOPFX
    {
        auto& loopfx = _database["mem"][memory_index]["LOOPFX"];
        loopFx_Sw->setChecked(loopfx["Sw"].get<int>());
        loopFx_Type->setCurrentIndex(loopfx["FxType"].get<int>());
        loopFx_ScatLen->setCurrentIndex(loopfx["ScatterLength"].get<int>());
        loopFx_ReptLen->setCurrentIndex(loopfx["RepeatLength"].get<int>());
        loopFx_Shift->setCurrentIndex(loopfx["ShiftShift"].get<int>());
        loopFx_Flick->setValue(loopfx["VinylFlickFlick"].get<int>());
    }

    // RHYTHM
    {
        auto& rhythm = _database["mem"][memory_index]["RHYTHM"];
        rhythm_Level->setValue(rhythm["Level"].get<int>());
        rhythm_Reverb->setValue(rhythm["Reverb"].get<int>());
        rhythm_Pattern->setCurrentIndex(rhythm["Pattern"].get<int>());
        rhythm_Variation->setCurrentIndex(rhythm["Variation"].get<int>());
        rhythm_VarChange->setCurrentIndex(rhythm["VariationChange"].get<int>());
        rhythm_Kit->setCurrentIndex(rhythm["Kit"].get<int>());
        rhythm_Beat->setCurrentIndex(rhythm["Beat"].get<int>());
        rhythm_Start->setCurrentIndex(rhythm["Start"].get<int>());
        rhythm_Stop->setCurrentIndex(rhythm["Stop"].get<int>());
        rhythm_RecCount->setCurrentIndex(rhythm["RecCount"].get<int>());
        rhythm_PlayCount->setCurrentIndex(rhythm["PlayCount"].get<int>());
        rhythm_Fill->setChecked(rhythm["Fill"].get<int>());
        rhythm_Part1->setChecked(rhythm["Part1"].get<int>());
        rhythm_Part2->setChecked(rhythm["Part2"].get<int>());
        rhythm_Part3->setChecked(rhythm["Part3"].get<int>());
        rhythm_Part4->setChecked(rhythm["Part4"].get<int>());
        rhythm_ToneLow->setValue(rhythm["ToneLow"].get<int>());
        rhythm_ToneHigh->setValue(rhythm["ToneHigh"].get<int>());
    }

    // CONTROL
    {
        auto& ctl = _database["mem"][memory_index]["CTL"];
        control_Pedal1->setCurrentIndex(ctl["Pedal1"].get<int>());
        control_Pedal2->setCurrentIndex(ctl["Pedal2"].get<int>());
        control_Pedal3->setCurrentIndex(ctl["Pedal3"].get<int>());
        control_Control1->setCurrentIndex(ctl["Ctl1"].get<int>());
        control_Control2->setCurrentIndex(ctl["Ctl2"].get<int>());
        control_Expression->setCurrentIndex(ctl["Exp"].get<int>());
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Level_changed(QSlider* slider)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (slider == track1_Level ? 0 : 1);
    int value = slider->value();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Level: " << value << std::endl;

    auto value_str = std::to_string(value);
    _database["mem"][memory_index]["TRACK"][track_index]["PlyLvl"] = value;

    slider->setToolTip(value_str.c_str());
    (slider == track1_Level ? label_track1_PlyLevel : label_track2_PlyLevel)->setText(value_str.c_str());
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Pan_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Pan ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Pan: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Pan"] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Start_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Start ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Start: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["StrtMod"] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Stop_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Stop ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Stop: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["StpMod"] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Measure_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Measure ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Measure: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Measure"] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Reverse_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Reverse ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", Rev: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Rev"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_LoopFx ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", LoopFx: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["LoopFx"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_OneShot_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_OneShot ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", OneShot: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["One"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopSync_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_LoopSync ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", LoopSync: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["LoopSync"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_TempoSync_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_TempoSync ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", TempoSync: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["TempoSync"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Input_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Input ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Input: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Input"] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Output_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Output ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Output: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Output"] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_ComboBox_changed(QComboBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["MASTER"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_CheckBox_changed(QCheckBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["MASTER"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_SpinBox_changed(QSpinBox* sb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = sb->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["MASTER"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_DoubleSpinBox_changed(QDoubleSpinBox* sb, const char* name, int factor)
{
    int memory_index = cb_Memory->currentIndex();
    auto value = sb->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["MASTER"][name] = static_cast<int>(value * factor);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_ComboBox_changed(QComboBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["MASTER"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_CheckBox_changed(QCheckBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["LOOPFX"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_SpinBox_changed(QSpinBox* sb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = sb->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["LOOPFX"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Rhythm_ComboBox_changed(QComboBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["RHYTHM"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Rhythm_CheckBox_changed(QCheckBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["RHYTHM"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Rhythm_Slider_changed(QSlider* s, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = s->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["RHYTHM"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Control_ComboBox_changed(QComboBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database["mem"][memory_index]["CTL"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::setFilename(const std::string& filename)
{
    _filename = filename;

    std::string title = "BOSS RC-500 - " + (_filename.empty() ? "[Untitled]" : _filename);
    _parent.setWindowTitle(title.c_str());
}
