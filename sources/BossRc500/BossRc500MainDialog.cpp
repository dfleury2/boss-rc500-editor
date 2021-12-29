#include "BossRc500MainDialog.hpp"
#include "BossRc500AssignDialog.hpp"
#include "BossRc500SystemDialog.hpp"
#include "BossRc500.hpp"

#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QInputDialog>
#include <QMenu>

#include <miniaudio/miniaudio.h>

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>

namespace {
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
    _parent.setFixedSize(_parent.width(), _parent.height());

    font_bold = _parent.font();
    font_bold.setWeight(QFont::Weight::Bold);

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

    auto themesMenu = new QMenu("Themes", toolMenu);
    for (auto filename : std::filesystem::directory_iterator("./resources/themes")) {
        QString stem;
#if WIN32
        stem = QString::fromWCharArray(filename.path().stem().c_str()); // wchar quick fix
#else
        stem = filename.path().stem().c_str();
#endif
        themesMenu->addAction(stem,
                [this, filename] {
                    on_ToolMenu_Themes(std::filesystem::absolute(filename));
                });
    }
    toolMenu->addMenu(themesMenu);

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
    BossRc500::Pan(track1_Pan);
    BossRc500::Pan(track2_Pan);

    // START
    BossRc500::Start(track1_Start);
    BossRc500::Start(track2_Start);

    // STOP
    BossRc500::Stop(track1_Stop);
    BossRc500::Stop(track2_Stop);

    // MEASURE
    BossRc500::Measure(track1_Measure);
    BossRc500::Measure(track2_Measure);

    // INPUT
    BossRc500::Input(track1_Input);
    BossRc500::Input(track2_Input);

    // OUTPUT
    BossRc500::Output(track1_Output);
    BossRc500::Output(track2_Output);

    // ----- RECORD -----
    BossRc500::DubMode(record_DubMode);
    BossRc500::RecAction(record_RecordAction);
    BossRc500::Quantize(record_Quantize);
    BossRc500::RecSource(record_AutoRecordSource);
    BossRc500::LoopLength(record_LoopLength);

    // ----- PLAY -----
    BossRc500::PlayMode(play_PlayMode);
    BossRc500::SingleChange(play_SingleChange);
    BossRc500::FadeTime(play_FadeTime);
    BossRc500::AllStart(play_AllStart);
    BossRc500::TrkChain(play_TrackChain);

    // ----- LOOP FX -----
    BossRc500::LoopFxType(loopFx_Type);
    BossRc500::ScatLen(loopFx_ScatLen);
    BossRc500::ReptLen(loopFx_ReptLen);
    BossRc500::Shift(loopFx_Shift);

    // ----- RHYTHM ----
    BossRc500::RhythmPattern(rhythm_Pattern);
    BossRc500::RhythmVariation(rhythm_Variation);
    BossRc500::RhythmVarChange(rhythm_VarChange);
    BossRc500::RhythmKit(rhythm_Kit);
    BossRc500::RhythmBeat(rhythm_Beat);
    BossRc500::RhythmStart(rhythm_Start);
    BossRc500::RhythmStop(rhythm_Stop);
    BossRc500::RhythmRecCount(rhythm_RecCount);
    BossRc500::RhythmPlayCount(rhythm_PlayCount);
    BossRc500::RhythmTone(rhythm_ToneLow);
    BossRc500::RhythmTone(rhythm_ToneHigh);

    // ----- CONTROL -----
    BossRc500::ControlPdlCtl(control_Pedal1);
    BossRc500::ControlPdlCtl(control_Pedal2);
    BossRc500::ControlPdlCtl(control_Pedal3);
    BossRc500::ControlPdlCtl(control_Control1);
    BossRc500::ControlPdlCtl(control_Control2);
    BossRc500::ControlExpr(control_Expression);
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
        auto beat = rhythm_Beat->currentData().value<QPoint>();
        auto loopFx_type = loopFx_Type->currentIndex();

        QDialog dialog;
        BossRc500AssignDialog assignDialog(dialog,
                _database_mem,
                cb_Memory->currentIndex(),
                beat,
                loopFx_type);
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
BossRc500MainDialog::on_ToolMenu_Themes(const std::filesystem::path& path)
{
    try {
        std::ifstream in(path);
        std::string style_sheet{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};

        qApp->setStyleSheet(style_sheet.c_str());
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
    _database_mem_default = _database_mem = ReadMemoryDatabase(filename);

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
    _database_sys_default = _database_sys = ReadSystemDatabase(filename);
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
    loopFx_ScatLen->setEnabled(BossRc500::IsScatter(loopFx_Type->currentIndex()));
    loopFx_ReptLen->setEnabled(BossRc500::IsRepeat(loopFx_Type->currentIndex()));
    loopFx_Shift->setEnabled(BossRc500::IsShift(loopFx_Type->currentIndex()));
    loopFx_Flick->setEnabled(BossRc500::IsVinyl(loopFx_Type->currentIndex()));
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
        auto beat = cb->currentData().value<QPoint>();

        // On beat change, we need to recompute the available values for Scat, Rep and Shift
        std::cout << "Beat signature: " << beat.x() << "/" << beat.y() << std::endl;
        BossRc500::ScatLen_EnableItems(loopFx_ScatLen, beat);
        BossRc500::ReptLen_EnableItems(loopFx_ReptLen, beat);
        BossRc500::ShiftLen_EnableItems(loopFx_Shift, beat);
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

        if (_database_mem["mem"][memory_index]["CTL"][name] != _database_mem_default["mem"][memory_index]["CTL"][name]) {
            cb->setFont(font_bold);
        }
        else {
            cb->setFont(_parent.font());
        }
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
