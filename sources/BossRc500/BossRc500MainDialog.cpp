#include "BossRc500MainDialog.hpp"
#include "BossRc500AssignDialog.hpp"
#include "BossRc500SystemDialog.hpp"

#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QInputDialog>
#include <QMenu>
#include <QStandardItemModel>

#include <miniaudio/miniaudio.h>

#include <iostream>
#include <initializer_list>
#include <filesystem>
#include <algorithm>

namespace {
// --------------------------------------------------------------------------
void
AddItemsToComboBox(QComboBox* cb, std::initializer_list<const char*> list)
{
    int i = 0; // By default, UserDate (QVariant) is the same as item index
    for (auto& item : list) {
        cb->addItem(item, i);
        ++i;
    }
}

// --------------------------------------------------------------------------
void
data_callback(ma_device* pDevice, void* pOutput, const void*, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder) {

        // Reading PCM frames will loop based on what we specified when called ma_data_source_set_looping().
        ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
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
    toolMenu->addAction("System...",   this, &BossRc500MainDialog::on_ToolMenu_System);
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

    play_FadeTime->setIconSize(QSize{32, 32});
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

    AddItemsToComboBox_LoopFx_ScatLen();
    AddItemsToComboBox_LoopFx_RepLen();
    AddItemsToComboBox_LoopFx_Shift();

    // ----- RHYTHM ----
    AddItemsToComboBox(rhythm_Pattern, {
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
    AddItemsToComboBox(rhythm_Variation, {"A", "B"});
    AddItemsToComboBox(rhythm_VarChange, {"MEASURE", "LOOP END"});
    AddItemsToComboBox(rhythm_Kit, {"Studio", "Live", "Light", "Heavy", "Rock", "Metal", "Jazz",
                                    "Brush", "Cajon", "Drum & Bass", "Dance", "Techno", "Dance Beats",
                                    "HipHop", "808+909"});


    // 2/4, 3/4, 4/4, 5/4, 6/4, 7/4
    for (auto note_count = 2; note_count <=7; ++note_count) { // X/4
        rhythm_Beat->addItem((std::to_string(note_count) + "/4").c_str(), QPoint{note_count, 4});
    }
    // 5/8, 6/8, 7/8, 8/8, 9/8, 10/8, 11/8,  12/8, 13/8, 14/8, 15/8
    for (auto note_count = 5; note_count <=15; ++note_count) { // X/8
        rhythm_Beat->addItem((std::to_string(note_count) + "/8").c_str(), QPoint{note_count, 8});
    }

    AddItemsToComboBox(rhythm_Start, {"LOOP START", "REC END", "BEFORE LOOP"});
    AddItemsToComboBox(rhythm_Stop, {"OFF", "LOOP STOP", "REC END"});
    AddItemsToComboBox(rhythm_RecCount, {"OFF", "1-MEASURE"});
    AddItemsToComboBox(rhythm_PlayCount, {"OFF", "1-MEASURE"});

    for (int i = -10; i <= 10; ++i) { // May be easier to understand if reversed here ?
        rhythm_ToneLow->addItem(((i > 0 ? "+" : "") + std::to_string(i)).c_str());
        rhythm_ToneHigh->addItem(((i > 0 ? "+" : "") + std::to_string(i)).c_str());
    }

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
BossRc500MainDialog::AddItemsToComboBox_LoopFx_ScatLen()
{
    loopFx_ScatLen->setIconSize(QSize{32, 32});

    loopFx_ScatLen->addItem("THRU");
    loopFx_ScatLen->addItem(QIcon("./resources/images/whole note.png"), "Whole Note", 8);
    loopFx_ScatLen->addItem(QIcon("./resources/images/half note dotted.png"), "Half Note Dotted", 7);
    loopFx_ScatLen->addItem(QIcon("./resources/images/half note.png"), "Half Note", 6);
    loopFx_ScatLen->addItem(QIcon("./resources/images/quarter note dotted.png"), "Quarter Note Dotted", 5);
    loopFx_ScatLen->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note", 4);
    loopFx_ScatLen->addItem(QIcon("./resources/images/quaver.png"), "Quaver", 3);
    loopFx_ScatLen->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver", 2);
    //loopFx_ScatLen->addItem(QIcon("./resources/images/demi semi-quaver.png"), "Demi Semi-quaver", 1);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::AddItemsToComboBox_LoopFx_RepLen()
{
    loopFx_ReptLen->setIconSize(QSize{32, 32});

    loopFx_ReptLen->addItem("THRU", 0);
    loopFx_ReptLen->addItem(QIcon("./resources/images/whole note.png"), "Whole Note", 8);
    loopFx_ReptLen->addItem(QIcon("./resources/images/half note dotted.png"), "Half Note Dotted", 7);
    loopFx_ReptLen->addItem(QIcon("./resources/images/half note.png"), "Half Note", 6);
    loopFx_ReptLen->addItem(QIcon("./resources/images/quarter note dotted.png"), "Quarter Note Dotted", 5);
    loopFx_ReptLen->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note", 4);
    loopFx_ReptLen->addItem(QIcon("./resources/images/quaver.png"), "Quaver", 3);
    loopFx_ReptLen->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver", 2);
    loopFx_ReptLen->addItem(QIcon("./resources/images/demi semi-quaver.png"), "Demi Semi-quaver", 1);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::AddItemsToComboBox_LoopFx_Shift()
{
    loopFx_Shift->setIconSize(QSize{32, 32});

    loopFx_Shift->addItem("THRU");
    //loopFx_Shift->addItem(QIcon("./resources/images/demi semi-quaver.png"), "Demi Semi-quaver", 1);
    loopFx_Shift->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver", 2);
    loopFx_Shift->addItem(QIcon("./resources/images/quaver.png"), "Quaver", 3);
    loopFx_Shift->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note", 4);
    loopFx_Shift->addItem(QIcon("./resources/images/quarter note dotted.png"), "Quarter Note Dotted", 5);
    loopFx_Shift->addItem(QIcon("./resources/images/half note.png"), "Half Note", 6);
    loopFx_Shift->addItem(QIcon("./resources/images/half note dotted.png"), "Half Note Dotted", 7);
    loopFx_Shift->addItem(QIcon("./resources/images/whole note.png"), "Whole Note", 8);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::EnableItemsToComboBox_LoopFx_ScatLen(const QPoint& beat)
{
    std::cout << "Beat signature: " << beat.x() << "/" << beat.y() << std::endl;

    auto current_index = loopFx_ScatLen->currentIndex();

    SetComboBoxItemEnabled(loopFx_ScatLen, 0, true); // THRU - Always active
    SetComboBoxItemEnabled(loopFx_ScatLen, 1, false); // Whole Note
    SetComboBoxItemEnabled(loopFx_ScatLen, 2, false); // Half Note Dotted
    SetComboBoxItemEnabled(loopFx_ScatLen, 3, beat.x() == 2 || beat.x() == 4 || beat.x() == 8); // Half Note
    SetComboBoxItemEnabled(loopFx_ScatLen, 4, beat.y() == 8 && beat.x() % 3 == 0); // Quarter Note Dotted
    SetComboBoxItemEnabled(loopFx_ScatLen, 5, beat.y() == 4 || (beat.y() == 8 && (beat.x() == 8 || beat.x() == 10 || beat.x() == 14))); // Quarter Note
    SetComboBoxItemEnabled(loopFx_ScatLen, 6, true); // Quaver
    SetComboBoxItemEnabled(loopFx_ScatLen, 7, true); // Semi-quaver

    if (!IsComboBoxItemEnabled(loopFx_ScatLen, current_index)) {
        loopFx_ScatLen->setCurrentIndex(6); // Quaver as default
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::EnableItemsToComboBox_LoopFx_RepLen(const QPoint& beat)
{
    // Number of notes per measure = beat.x();
    // Note duration = beat.y();
    std::cout << "Beat signature: " << beat.x() << "/" << beat.y() << std::endl;

    auto current_index = loopFx_ReptLen->currentIndex();

    SetComboBoxItemEnabled(loopFx_ReptLen, 0, true); // THRU - Always active
    SetComboBoxItemEnabled(loopFx_ReptLen, 1, beat.x() == beat.y()); // Whole Note
    SetComboBoxItemEnabled(loopFx_ReptLen, 2, beat.x() % 3 == 0 && beat.x() != 9 && beat.x() != 15); // Half Note Dotted
    SetComboBoxItemEnabled(loopFx_ReptLen, 3, beat.x() * 2 == beat.y()); // Half Note
    SetComboBoxItemEnabled(loopFx_ReptLen, 4, beat.y() == 8 && beat.x() % 3 == 0 && beat.x() > beat.y()); // Quarter Note Dotted
    SetComboBoxItemEnabled(loopFx_ReptLen, 5, beat.y() == 4); // Quarter Note
    SetComboBoxItemEnabled(loopFx_ReptLen, 6, true); // Quaver
    SetComboBoxItemEnabled(loopFx_ReptLen, 7, true); // Semi-quaver
    SetComboBoxItemEnabled(loopFx_ReptLen, 8, true); // Demi Semi-quaver

    if (!IsComboBoxItemEnabled(loopFx_ReptLen, current_index)) {
        loopFx_ReptLen->setCurrentIndex(6); // Quaver as default
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::EnableItemsToComboBox_LoopFx_Shift(const QPoint& beat)
{
    std::cout << "Beat signature: " << beat.x() << "/" << beat.y() << std::endl;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_MemoryPrevious, &QPushButton::pressed, this, &BossRc500MainDialog::on_memory_previous);
    QObject::connect(button_MemoryNext, &QPushButton::pressed, this, &BossRc500MainDialog::on_memory_next);
    QObject::connect(button_Copy, &QPushButton::pressed, this, &BossRc500MainDialog::on_copy);
    QObject::connect(button_rhythm_Play, &QPushButton::pressed, this, &BossRc500MainDialog::on_rhythm_play);

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
            this, [this] { on_LoopFx_FxType_changed(); });

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
            this,
            [this] {
                on_Rhythm_Slider_changed(rhythm_Level, "Level");
                label_rhythm_LevelValue->setText(std::to_string(rhythm_Level->value()).c_str());
            });

    QObject::connect(rhythm_Reverb, &QSlider::valueChanged,
            this,
            [this] {
                on_Rhythm_Slider_changed(rhythm_Reverb, "Reverb");
                label_rhythm_ReverbValue->setText(std::to_string(rhythm_Reverb->value()).c_str());
            });

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

    QObject::connect(rhythm_ToneLow, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_ToneLow, "ToneLow"); });
    QObject::connect(rhythm_ToneHigh, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_ToneHigh, "ToneHigh"); });

    // Control callbacks
    auto ConnectComboBox_Control = [&](QComboBox* cb, const char* name) {
        QObject::connect(cb, &QComboBox::currentIndexChanged,
                this, [this, cb, name] {
                    on_Control_ComboBox_changed(cb, name);
                });
    };

    ConnectComboBox_Control(control_Pedal1, "Pedal1");
    ConnectComboBox_Control(control_Pedal2, "Pedal2");
    ConnectComboBox_Control(control_Pedal3, "Pedal3");
    ConnectComboBox_Control(control_Control1, "Ctl1");
    ConnectComboBox_Control(control_Control2, "Ctl2");
    ConnectComboBox_Control(control_Expression, "Exp");
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_New()
{
    try {
        setDirname("");
        load_database_sys("./resources/templates/SYSTEM_DEFAULT.RC0");
        load_database_mem("./resources/templates/MEMORY_DEFAULT.RC0");

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
        auto dirname = QFileDialog::getExistingDirectory(&_parent,
                tr("Open a DATA directory"), "").toStdString();
        if (!dirname.empty()) {

            setDirname(dirname);

            if (auto filename_sys = dirname + "/SYSTEM1.RC0"; std::filesystem::exists(filename_sys)) {
                load_database_sys(filename_sys);
            }

            if (auto filename_nem = dirname + "/MEMORY1.RC0"; std::filesystem::exists(filename_nem)) {
                load_database_mem(filename_nem);

                cb_Memory->setCurrentIndex(0);
                load_memory(cb_Memory->currentIndex());
            }
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
        setDirname("");
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_Save(bool askDirname)
{
    try {
        if (_dirname.empty() || askDirname) {
            auto dirname = QFileDialog::getExistingDirectory(&_parent,
                    tr("Save to MEMORY/SYSTEM files to a directory"), "").toStdString();
            if (dirname.empty()) {
                return;
            }

            setDirname(dirname);
        }

        if (auto response = QMessageBox::question(nullptr,
                    "Write changes to directory ?",
                    QString::fromStdString("Do you want to write to files into:\n" + _dirname));
                response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        WriteMemoryDatabase(_database_mem, _dirname + "/MEMORY1.RC0");
        WriteMemoryDatabase(_database_mem, _dirname + "/MEMORY2.RC0");

        WriteSystemDatabase(_database_sys, _dirname + "/SYSTEM1.RC0");
        WriteSystemDatabase(_database_sys, _dirname + "/SYSTEM2.RC0");

        QMessageBox(QMessageBox::Information, "Information", "MEMORY and SYSTEM files have been written.").exec();
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
        BossRc500AssignDialog assignDialog(dialog, _database_mem, cb_Memory->currentIndex());
        dialog.setWindowTitle("BOSS RC-500 - Assign");
        dialog.setModal(true);
        dialog.exec();
        if (assignDialog.apply) {
            _database_mem = std::move(assignDialog.database);
        }

    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_System()
{
    try {
        QDialog dialog;
        BossRc500SystemDialog systemDialog(dialog, _database_sys);
        dialog.setWindowTitle("BOSS RC-500 - System");
        dialog.setModal(true);
        dialog.exec();
        if (systemDialog.apply) {
            _database_sys = std::move(systemDialog.database);

            load_memory(cb_Memory->currentIndex());
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
        const auto& slot = _database_mem["mem"][memory_slot - 1];

        for (int i = copy_from_slot; i <= copy_to_slot; ++i) {
            _database_mem["mem"][i - 1] = slot;
            // Restore the memory id of the copied slot
            _database_mem["mem"][i - 1]["id"] = i - 1;
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
                _database_mem["mem"][memory_index]["name"].get<std::string>().c_str(), &ok).toStdString();
        if (ok) {
            _database_mem["mem"][memory_index]["name"] = text;

            // Update NAME.C01 to NAME.C12
            for (int i = 0; i < 12; ++i) {
                std::string cxx = "C";
                cxx += (i + 1 < 10 ? "0" : "") + std::to_string(i + 1);

                int value = (i < text.size() ? text[i] : 32);
                _database_mem["mem"][memory_index]["NAME"][cxx] = (value >= 32 && value <= 127 ? value : '_');
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
void
BossRc500MainDialog::on_memory_previous()
{
    int memory_index = cb_Memory->currentIndex();
    if (memory_index > 0) {
        cb_Memory->setCurrentIndex(memory_index - 1);
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_memory_next()
{
    int memory_index = cb_Memory->currentIndex();
    if (memory_index < 98) {
        cb_Memory->setCurrentIndex(memory_index + 1);
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_memory_changed()
{
    load_memory(cb_Memory->currentIndex());
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_rhythm_play()
{
    std::string drumkit_filename = "./resources/drumkits/";
    drumkit_filename += rhythm_Pattern->currentText().toStdString() + "_";
    drumkit_filename += rhythm_Variation->currentText().toStdString() + "_";

    auto beat = rhythm_Beat->currentText().toStdString();
    beat.erase(std::remove(begin(beat), end(beat), '/'), end(beat));

    drumkit_filename += beat + ".wav";

    ma_decoder decoder;
    ma_result result = ma_decoder_init_file(drumkit_filename.c_str(), nullptr, &decoder);
    if (result != MA_SUCCESS) {
        std::cout << "Failed to open file: " << drumkit_filename << std::endl;
        return;
    }

    /*
    A decoder is a data source which means we just use ma_data_source_set_looping() to set the
    looping state. We will read data using ma_data_source_read_pcm_frames() in the data callback.
    */
    ma_data_source_set_looping(&decoder, MA_TRUE);

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    ma_device device;
    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        std::cout << "Failed to open playback device." << std::endl;
        ma_decoder_uninit(&decoder);
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start playback device." << std::endl;
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }

    QMessageBox(QMessageBox::Information, "Information", drumkit_filename.c_str()).exec();

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::load_database_mem(const std::string& filename)
{
    _database_mem = ReadMemoryDatabase(filename);

    // Add name to memory index
    for (int i = 1; i <= 99; ++i) {
        auto index = std::to_string(i);
        if (auto found_name = _database_mem["mem"][i - 1].find("name");
                found_name != _database_mem["mem"][i - 1].end()) {
            index += " - " + found_name->get<std::string>();
        }
        cb_Memory->setItemText(i - 1, index.c_str());
    }
}
// --------------------------------------------------------------------------
void
BossRc500MainDialog::load_database_sys(const std::string& filename)
{
    _database_sys = ReadSystemDatabase(filename);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::load_memory(int memory_index)
{
    { // TRACK 1
        auto& track1 = _database_mem["mem"][memory_index]["TRACK"][0];

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
        auto& track2 = _database_mem["mem"][memory_index]["TRACK"][1];

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
        auto& master = _database_mem["mem"][memory_index]["MASTER"];
        record_DubMode->setCurrentIndex(master["DubMode"].get<int>());

        // RECORD
        record_DubMode->setCurrentIndex(master["DubMode"].get<int>());
        record_RecordAction->setCurrentIndex(master["RecAction"].get<int>());

        int recQuantize = master["RecQuantize"].get<int>();
        if (recQuantize == 2) recQuantize = 1;
        record_Quantize->setCurrentIndex(recQuantize);

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
        auto& loopfx = _database_mem["mem"][memory_index]["LOOPFX"];
        loopFx_Sw->setChecked(loopfx["Sw"].get<int>());
        loopFx_Type->setCurrentIndex(loopfx["FxType"].get<int>());

        // These one are reverted from 8 to 1
        auto scatter = loopfx["ScatterLength"].get<int>();
        loopFx_ScatLen->setCurrentIndex(scatter ? 9 - scatter : 0);

        auto repeat = loopfx["RepeatLength"].get<int>();
        loopFx_ReptLen->setCurrentIndex(repeat ? 9 - repeat : 0);

        // Index 1 is not available (0, 2, 3, 4, ....)
        auto shift = loopfx["ShiftShift"].get<int>();
        auto found_Shift = loopFx_Shift->findData(shift);
        loopFx_Shift->setCurrentIndex(found_Shift != -1 ? found_Shift : 2);
        //std::cout << "Found shift value: " << shift << " at index: " << found_Shift << std::endl;

        loopFx_Flick->setValue(loopfx["VinylFlickFlick"].get<int>());
    }

    // RHYTHM
    {
        auto& rhythm = _database_mem["mem"][memory_index]["RHYTHM"];
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
        rhythm_ToneLow->setCurrentIndex(rhythm["ToneLow"].get<int>());
        rhythm_ToneHigh->setCurrentIndex(rhythm["ToneHigh"].get<int>());
    }

    // CONTROL
    // These controls are effective only if the associated PREF is set to MEMORY
    {
        auto& ctl = _database_mem["mem"][memory_index]["CTL"];
        auto& pref  = _database_sys["sys"]["PREF"];
        auto& sys_ctl  = _database_sys["sys"]["CTL"];

        auto SetControl = [&](QComboBox* cb, const char* pref_name, const char* ctl_name) {
            cb->setEnabled(pref[pref_name].get<int>() == 0);
            if (cb->isEnabled()) {
                cb->setCurrentIndex(ctl[ctl_name].get<int>());
            }
            else {
                cb->setCurrentIndex(sys_ctl[ctl_name].get<int>());
            }
        };

        SetControl(control_Pedal1, "Pdl1", "Pedal1");
        SetControl(control_Pedal2, "Pdl2", "Pedal2");
        SetControl(control_Pedal3, "Pdl3", "Pedal3");
        SetControl(control_Control1, "Ctl1", "Ctl1");
        SetControl(control_Control2, "Ctl2", "Ctl2");
        SetControl(control_Expression, "Exp", "Exp");
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
    _database_mem["mem"][memory_index]["TRACK"][track_index]["PlyLvl"] = value;

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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["Pan"] = value;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["StrtMod"] = value;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["StpMod"] = value;
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

    int measMod = 0, measLen = 0, measBtLp = 0;
    if (value == 0) { // AUTO
        measMod = measLen = measBtLp = 0;
    }
    else if (value == 1) { // FREE
        measMod = 1;
        measLen = measBtLp = 0;
    }
    else if (value >= 2 && value <= 7) { // NOTE mode
        measMod = 2;
        measLen = 1;
        measBtLp = value - 1;
    }
    else { // MEASURE mode
        measMod = 2;
        measLen = value - 7;
        measBtLp = 0;
    }

    _database_mem["mem"][memory_index]["TRACK"][track_index]["Measure"] = value;
    _database_mem["mem"][memory_index]["TRACK"][track_index]["MeasMod"] = measMod;
    _database_mem["mem"][memory_index]["TRACK"][track_index]["MeasLen"] = measLen;
    _database_mem["mem"][memory_index]["TRACK"][track_index]["MeasBtLp"] = measBtLp;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["Rev"] = is_checked;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["LoopFx"] = is_checked;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["One"] = is_checked;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["LoopSync"] = is_checked;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["TempoSync"] = is_checked;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["Input"] = value;
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

    _database_mem["mem"][memory_index]["TRACK"][track_index]["Output"] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_ComboBox_changed(QComboBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = cb->currentIndex();

    // Some ugly ack for Quantize
    // NOTE: may be a usage for userData and QVariant here, but too long to refactor
    if (cb == record_Quantize) {
        if (value == 1) { // MEASURE
            value = 2;
        }
    }

    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["MASTER"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_CheckBox_changed(QCheckBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["MASTER"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_SpinBox_changed(QSpinBox* sb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = sb->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["MASTER"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Master_DoubleSpinBox_changed(QDoubleSpinBox* sb, const char* name, int factor)
{
    int memory_index = cb_Memory->currentIndex();
    auto value = sb->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["MASTER"][name] = static_cast<int>(value * factor);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_FxType_changed()
{
    on_LoopFx_ComboBox_changed(loopFx_Type, "FxType");

    // Discard some combobox
    std::string fxType = loopFx_Type->currentText().toStdString();
    loopFx_ScatLen->setEnabled(fxType.find("SCATTER") != std::string::npos);
    loopFx_ReptLen->setEnabled(fxType.find("REPEAT") != std::string::npos);
    loopFx_Shift->setEnabled(fxType.find("SHIFT") != std::string::npos);
    loopFx_Flick->setEnabled(fxType.find("VINYL") != std::string::npos);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_ComboBox_changed(QComboBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    auto i = cb->currentData().value<int>();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << i << std::endl;
    _database_mem["mem"][memory_index]["LOOPFX"][name] = i;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_CheckBox_changed(QCheckBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["LOOPFX"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_LoopFx_SpinBox_changed(QSpinBox* sb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = sb->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["LOOPFX"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Rhythm_ComboBox_changed(QComboBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["RHYTHM"][name] = value;

    if (cb == rhythm_Beat) {
        auto p = cb->currentData().value<QPoint>();

        // On beat change, we need to recompute the available values for Scat, Rep and Shift
        EnableItemsToComboBox_LoopFx_ScatLen(p);
        EnableItemsToComboBox_LoopFx_RepLen(p);
        EnableItemsToComboBox_LoopFx_Shift(p);
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Rhythm_CheckBox_changed(QCheckBox* cb, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["RHYTHM"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Rhythm_Slider_changed(QSlider* s, const char* name)
{
    int memory_index = cb_Memory->currentIndex();
    int value = s->value();
    std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
    _database_mem["mem"][memory_index]["RHYTHM"][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_Control_ComboBox_changed(QComboBox* cb, const char* name)
{
    // Change the database only if cb is enabled // MEMORY mode in sys.PREF
    if (cb->isEnabled()) {
        int memory_index = cb_Memory->currentIndex();
        int value = cb->currentIndex();
        std::cout << "Memory: " << (memory_index + 1) << ", " << name << ": " << value << std::endl;
        _database_mem["mem"][memory_index]["CTL"][name] = value;
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::setDirname(const std::string& dirname)
{
    _dirname = dirname;

    std::string title = "BOSS RC-500 - " + (_dirname.empty() ? "[Untitled]" : _dirname);
    _parent.setWindowTitle(title.c_str());
}
