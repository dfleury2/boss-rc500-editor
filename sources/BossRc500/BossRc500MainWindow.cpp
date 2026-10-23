#include "BossRc500MainWindow.hpp"
#include "BossRc500.hpp"
#include "BossRc500AdvancedCopyDialog.hpp"
#include "BossRc500SystemDialog.hpp"
#include "Designer/ui_Boss-rc500-text.h"
#include <BossRc500/config.h>

#include <QActionGroup>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QStyleFactory>
#include <QUndoView>

// #include <miniaudio/miniaudio.h>

#include <algorithm>
#include <filesystem>
#include <iostream>

namespace {
// --------------------------------------------------------------------------
// void data_callback(ma_device* pDevice, void* pOutput, const void*, ma_uint32 frameCount)
// {
//     if (auto* pDecoder = static_cast<ma_decoder*>(pDevice->pUserData)) {
//         // Reading PCM frames will loop based on what we specified when called ma_data_source_set_looping().
//         ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
//     }
// }

}   // namespace

// --------------------------------------------------------------------------
BossRc500MainWindow::BossRc500MainWindow() : Ui_MainWindow()
{
    setup();
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::setup()
{
    setupUi(this);

    setLogo("500");
    label_Version->setText(PROJECT_VERSION);

    // Add some tweaks...
    setFixedSize(width(), height());

    const auto fileMenu = new QMenu("File", menuBar());
    fileMenu->addAction("New", QKeySequence::New, this, &BossRc500MainWindow::on_ToolMenu_New);
    fileMenu->addAction("Open...", QKeySequence::Open, this, &BossRc500MainWindow::on_ToolMenu_Open);
    fileMenu->addSeparator();
    fileMenu->addAction("Save", QKeySequence::Save, this, &BossRc500MainWindow::on_ToolMenu_Save);
    fileMenu->addAction("Save as...", QKeySequence::SaveAs, [this] { on_ToolMenu_Save(true); });

    fileMenu->addSeparator();
    fileMenu->addAction("Save as preset...", [this] { on_ToolMenu_PresetSave(); });

    _presetLoadMenu = new QMenu("Load Preset", fileMenu);
    loadPresets();
    fileMenu->addMenu(_presetLoadMenu);

    fileMenu->addSeparator();
    fileMenu->addAction("Show/Hide Assign", [this] {
        if (_assign_dialog->isHidden()) {
            _assign_dialog->show();
        }
        else {
            _assign_dialog->hide();
        }
    });
    fileMenu->addSeparator();
    fileMenu->addAction("System...", this, &BossRc500MainWindow::on_ToolMenu_System);
    fileMenu->addSeparator();

    {
        _preferences_themes_group = new QActionGroup(this);
        _preferences_themes_group->setExclusive(true);

        // Add the default theme (Default.css)
        const auto themesMenu = new QMenu("Themes", fileMenu);

        const auto default_themes =
            themesMenu->addAction("Default", [this] { on_ToolMenu_Themes(BossRc500::Resources::Themes() + "/Default.css"); });
        default_themes->setCheckable(true);
        default_themes->setChecked(true);
        _preferences_themes_group->addAction(default_themes);

        for (auto&& file_info : QDir{BossRc500::Resources::Themes()}.entryInfoList(QDir::Files)) {
            spdlog::info("Loading theme file [{}]", file_info.absoluteFilePath().toStdString());

            if (file_info.completeSuffix() == "css" && file_info.baseName() != "Default") {
                auto filename = file_info.absoluteFilePath();
                spdlog::info("Add theme file [{}]", filename.toStdString());

                auto theme = themesMenu->addAction(file_info.baseName(), [this, filename] { on_ToolMenu_Themes(filename); });
                theme->setCheckable(true);
                _preferences_themes_group->addAction(theme);
            }
        }

        fileMenu->addMenu(themesMenu);
    }

    {
        _preferences_language_group = new QActionGroup(this);
        _preferences_language_group->setExclusive(true);

        const auto preferencesMenu = new QMenu("Preferences", fileMenu);
        const auto default_english = preferencesMenu->addAction("Default", [this] {
            _language = "";
            add_tooltips();
        });
        default_english->setCheckable(true);
        default_english->setChecked(true);
        _preferences_language_group->addAction(default_english);

        for (auto&& l : BossRc500::Configuration::Instance().languages) {
            auto lang = preferencesMenu->addAction(l.first, [this, code = l.second] {
                _language = code;
                add_tooltips();
            });
            lang->setCheckable(true);
            _preferences_language_group->addAction(lang);
        }

        fileMenu->addMenu(preferencesMenu);
    }
#ifndef APPLE
    // Apple will use application menu to quit
    fileMenu->addSeparator();
    fileMenu->addAction("Quit", QKeySequence::Quit, [] { QApplication::exit(); });
#endif
    menuBar()->addMenu(fileMenu);

    const auto editMenu = new QMenu("Edit", menuBar());
    editMenu->addAction("Undo", QKeySequence::Undo, [this] { _stack.undo(); });
    editMenu->addAction("Redo", QKeySequence::Redo, [this] { _stack.redo(); });
    fileMenu->addSeparator();
    editMenu->addAction("Show/Hide Stack", [this] {
        if (_stack_dialog->isHidden()) {
            _stack_dialog->show();
        }
        else {
            _stack_dialog->hide();
        }
    });
    menuBar()->addMenu(editMenu);

    add_tooltips();
    add_combo_items();
    add_callbacks();

    {
        // Assign
        _assign_dialog = new QDialog(this);
        _boss_assign_dialog = new BossRc500AssignDialog(*_assign_dialog);
        _assign_dialog->setWindowTitle("RC-500 LOOP STATION - Assign");
    }

    {
        // Undo/Redo
        _stack_dialog = new QDialog(this);
        _stack_dialog->setLayout(new QGridLayout);

        const auto view = new QUndoView(_stack_dialog);
        view->setStack(&_stack);
        _stack_dialog->layout()->addWidget(view);

        _stack_dialog->setMinimumSize(250, 400);
    }

    // Start with a default MEMORY file
    on_ToolMenu_New();

    // No more drum kits...
    button_rhythm_Play->hide();
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::setLogo(const char* boss_version) const
{
    const auto logo = fmt::format(
        R"(<html><head/><body><p><span style=" font-size:26pt; font-weight:600; font-style:italic; vertical-align:super;">LOOP STATION</span><span style=" font-size:28pt; font-weight:600; font-style:italic;">RC-{} </span></p></body></html>)",
        boss_version);
    label_Logo->setText(logo.c_str());
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::add_tooltips() const
{
    // Add tooltips
#if QT_CONFIG(tooltip)
    try {
        BossRc500::Tooltips tooltips(_language);

        track1_Reverse->setToolTip(tooltips.track_Reverse());
        track1_LoopFx->setToolTip(tooltips.track_LoopFx());
        track1_OneShot->setToolTip(tooltips.track_OneShot());
        track1_Level->setToolTip(tooltips.track_Level());
        track1_Pan->setToolTip(tooltips.track_Pan());
        track1_Start->setToolTip(tooltips.track_Start());
        track1_Stop->setToolTip(tooltips.track_Stop());
        track1_Measure->setToolTip(tooltips.track_Measure());
        track1_LoopSync->setToolTip(tooltips.track_LoopSync());
        track1_TempoSync->setToolTip(tooltips.track_TempoSync());
        track1_Input->setToolTip(tooltips.track_Input());
        track1_Output->setToolTip(tooltips.track_Output());

        track2_Reverse->setToolTip(tooltips.track_Reverse());
        track2_LoopFx->setToolTip(tooltips.track_LoopFx());
        track2_OneShot->setToolTip(tooltips.track_OneShot());
        track2_Level->setToolTip(tooltips.track_Level());
        track2_Pan->setToolTip(tooltips.track_Pan());
        track2_Start->setToolTip(tooltips.track_Start());
        track2_Stop->setToolTip(tooltips.track_Stop());
        track2_Measure->setToolTip(tooltips.track_Measure());
        track2_LoopSync->setToolTip(tooltips.track_LoopSync());
        track2_TempoSync->setToolTip(tooltips.track_TempoSync());
        track2_Input->setToolTip(tooltips.track_Input());
        track2_Output->setToolTip(tooltips.track_Output());

        record_DubMode->setToolTip(tooltips.master_rec_DubMode());
        record_RecordAction->setToolTip(tooltips.master_rec_RecAction());
        record_Quantize->setToolTip(tooltips.master_rec_Quantize());
        record_AutoRecord->setToolTip(tooltips.master_rec_AutoRec());
        record_AutoRecordSensitivity->setToolTip(tooltips.master_rec_ARecSens());
        record_AutoRecordSource->setToolTip(tooltips.master_rec_ARecSrc());
        record_LoopLength->setToolTip(tooltips.master_rec_LoopLength());

        play_PlayMode->setToolTip(tooltips.master_play_PlayMode());
        play_SingleChange->setToolTip(tooltips.master_play_SinglChange());
        play_Level->setToolTip(tooltips.master_play_Level());
        play_FadeTime->setToolTip(tooltips.master_play_FadeTime());
        play_AllStart->setToolTip(tooltips.master_play_AllStart());
        play_TrackChain->setToolTip(tooltips.master_play_TrkChain());

        loopFx_Sw->setToolTip(tooltips.loopfx_LoopFx());
        loopFx_Type->setToolTip(tooltips.loopfx_Type());
        loopFx_ScatLen->setToolTip(tooltips.loopfx_ScatLen());
        loopFx_ReptLen->setToolTip(tooltips.loopfx_ReptLen());
        loopFx_Shift->setToolTip(tooltips.loopfx_Shift());
        loopFx_Flick->setToolTip(tooltips.loopfx_Flick());

        rhythm_Level->setToolTip(tooltips.rhythm_Level());
        rhythm_Reverb->setToolTip(tooltips.rhythm_Reverb());
        rhythm_Pattern->setToolTip(tooltips.rhythm_Pattern());
        rhythm_Variation->setToolTip(tooltips.rhythm_Variation());
        rhythm_VarChange->setToolTip(tooltips.rhythm_VarChange());
        rhythm_Kit->setToolTip(tooltips.rhythm_Kit());
        rhythm_Beat->setToolTip(tooltips.rhythm_Beat());
        rhythm_Start->setToolTip(tooltips.rhythm_Start());
        rhythm_Stop->setToolTip(tooltips.rhythm_Stop());
        rhythm_RecCount->setToolTip(tooltips.rhythm_RecCount());
        rhythm_PlayCount->setToolTip(tooltips.rhythm_PlayCount());
        rhythm_Fill->setToolTip(tooltips.rhythm_Fill());
        rhythm_Part1->setToolTip(tooltips.rhythm_Part14());
        rhythm_Part2->setToolTip(tooltips.rhythm_Part14());
        rhythm_Part3->setToolTip(tooltips.rhythm_Part14());
        rhythm_Part4->setToolTip(tooltips.rhythm_Part14());
        rhythm_ToneLow->setToolTip(tooltips.rhythm_ToneLow());
        rhythm_ToneHigh->setToolTip(tooltips.rhythm_ToneHigh());

        control_Expression->setToolTip(tooltips.control_Exp());
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

#endif   // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::add_combo_items() const
{
    // MEMORY
    for (int i = 1; i <= 99; ++i) {
        cb_Memory->addItem(fmt::format("{} - {}", i, _database_memory.memories[i - 1].name.to_string(i-1)).c_str());
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
    BossRc500::RhythmState(rhythm_State);

    // ----- CONTROL -----
    BossRc500::ControlPdlCtl(control_Pedal1);
    BossRc500::ControlPdlCtl(control_Pedal2);
    BossRc500::ControlPdlCtl(control_Pedal3);
    BossRc500::ControlPdlCtl(control_Control1);
    BossRc500::ControlPdlCtl(control_Control2);
    BossRc500::ControlExpr(control_Expression);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_MemoryPrevious, &QPushButton::pressed, this, &BossRc500MainWindow::on_memory_previous);
    QObject::connect(button_MemoryNext, &QPushButton::pressed, this, &BossRc500MainWindow::on_memory_next);
    QObject::connect(button_Copy, &QPushButton::pressed, this, &BossRc500MainWindow::on_copy);
    QObject::connect(button_AdvancedCopy, &QPushButton::pressed, this, &BossRc500MainWindow::on_advanced_copy);
    QObject::connect(button_rhythm_Play, &QPushButton::pressed, this, &BossRc500MainWindow::on_rhythm_play);

    QObject::connect(cb_Memory, &QComboBox::currentIndexChanged, this, &BossRc500MainWindow::on_memory_changed);
    QObject::connect(button_Edit, &QPushButton::pressed, this, &BossRc500MainWindow::on_edit);
    QObject::connect(button_pdlctlHelp, &QPushButton::pressed, this, &BossRc500MainWindow::on_control_pdlctl_help);

    // Track 1/2 callbacks
    QObject::connect(track1_Level, &QSlider::valueChanged, this, [this] { on_Level_changed(track1_Level); });
    QObject::connect(track2_Level, &QSlider::valueChanged, this, [this] { on_Level_changed(track2_Level); });

    QObject::connect(track1_Reverse, &QCheckBox::checkStateChanged, this, [this] { on_Reverse_changed(track1_Reverse); });
    QObject::connect(track2_Reverse, &QCheckBox::checkStateChanged, this, [this] { on_Reverse_changed(track2_Reverse); });

    QObject::connect(track1_LoopFx, &QCheckBox::checkStateChanged, this, [this] { on_LoopFx_changed(track1_LoopFx); });
    QObject::connect(track2_LoopFx, &QCheckBox::checkStateChanged, this, [this] { on_LoopFx_changed(track2_LoopFx); });

    QObject::connect(track1_OneShot, &QCheckBox::checkStateChanged, this, [this] { on_OneShot_changed(track1_OneShot); });
    QObject::connect(track2_OneShot, &QCheckBox::checkStateChanged, this, [this] { on_OneShot_changed(track2_OneShot); });

    QObject::connect(track1_Pan, &QComboBox::currentIndexChanged, this, [this] { on_Pan_changed(track1_Pan); });
    QObject::connect(track2_Pan, &QComboBox::currentIndexChanged, this, [this] { on_Pan_changed(track2_Pan); });

    QObject::connect(track1_Start, &QComboBox::currentIndexChanged, this, [this] { on_Start_changed(track1_Start); });
    QObject::connect(track2_Start, &QComboBox::currentIndexChanged, this, [this] { on_Start_changed(track2_Start); });

    QObject::connect(track1_Stop, &QComboBox::currentIndexChanged, this, [this] { on_Stop_changed(track1_Stop); });
    QObject::connect(track2_Stop, &QComboBox::currentIndexChanged, this, [this] { on_Stop_changed(track2_Stop); });

    QObject::connect(track1_Measure, &QComboBox::currentIndexChanged, this, [this] { on_Measure_changed(track1_Measure); });
    QObject::connect(track2_Measure, &QComboBox::currentIndexChanged, this, [this] { on_Measure_changed(track2_Measure); });

    QObject::connect(track1_LoopSync, &QCheckBox::checkStateChanged, this, [this] { on_LoopSync_changed(track1_LoopSync); });
    QObject::connect(track2_LoopSync, &QCheckBox::checkStateChanged, this, [this] { on_LoopSync_changed(track2_LoopSync); });

    QObject::connect(track1_TempoSync, &QCheckBox::checkStateChanged, this, [this] { on_TempoSync_changed(track1_TempoSync); });
    QObject::connect(track2_TempoSync, &QCheckBox::checkStateChanged, this, [this] { on_TempoSync_changed(track2_TempoSync); });

    QObject::connect(track1_Input, &QComboBox::currentIndexChanged, this, [this] { on_Input_changed(track1_Input); });
    QObject::connect(track2_Input, &QComboBox::currentIndexChanged, this, [this] { on_Input_changed(track2_Input); });

    QObject::connect(track1_Output, &QComboBox::currentIndexChanged, this, [this] { on_Output_changed(track1_Output); });
    QObject::connect(track2_Output, &QComboBox::currentIndexChanged, this, [this] { on_Output_changed(track2_Output); });

    // Master Record callbacks
    QObject::connect(record_DubMode, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(record_DubMode, &BossRC::Master::DubMode); });

    QObject::connect(record_RecordAction, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(record_RecordAction, &BossRC::Master::RecAction); });

    QObject::connect(record_Quantize, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(record_Quantize, &BossRC::Master::RecQuantize); });

    QObject::connect(record_AutoRecord, &QCheckBox::checkStateChanged, this,
                     [this] { on_Master_CheckBox_changed(record_AutoRecord, &BossRC::Master::AutoRec); });

    QObject::connect(record_AutoRecordSensitivity, &QSpinBox::valueChanged, this,
                     [this] { on_Master_SpinBox_changed(record_AutoRecordSensitivity, &BossRC::Master::AutoRecSens); });

    QObject::connect(record_AutoRecordSource, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(record_AutoRecordSource, &BossRC::Master::AutoRecSrc); });

    QObject::connect(record_LoopLength, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(record_LoopLength, &BossRC::Master::LpLen); });

    // Master Play callbacks
    QObject::connect(play_Tempo, &QDoubleSpinBox::valueChanged, this,
                     [this] { on_Master_DoubleSpinBox_changed(play_Tempo, &BossRC::Master::Tempo, 10); });

    QObject::connect(play_PlayMode, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(play_PlayMode, &BossRC::Master::PlayMode); });

    QObject::connect(play_SingleChange, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(play_SingleChange, &BossRC::Master::SinglPlayeChange); });

    QObject::connect(play_Level, &QSpinBox::valueChanged, this, [this] { on_Master_SpinBox_changed(play_Level, &BossRC::Master::Level); });

    QObject::connect(play_FadeTime, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(play_FadeTime, &BossRC::Master::FadeTime); });

    QObject::connect(play_AllStart, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(play_AllStart, &BossRC::Master::AllStart); });

    QObject::connect(play_TrackChain, &QComboBox::currentIndexChanged, this,
                     [this] { on_Master_ComboBox_changed(play_TrackChain, &BossRC::Master::TrackChain); });

    // LoopFX callbacks
    QObject::connect(loopFx_Sw, &QCheckBox::checkStateChanged, this,
                     [this] { on_LoopFx_CheckBox_changed(loopFx_Sw, &BossRC::LoopFxCfg::Sw); });

    QObject::connect(loopFx_Type, &QComboBox::currentIndexChanged, this, [this] { on_LoopFx_FxType_changed(); });

    QObject::connect(loopFx_ScatLen, &QComboBox::currentIndexChanged, this,
                     [this] { on_LoopFx_ComboBox_changed(loopFx_ScatLen, &BossRC::LoopFxCfg::ScatterLength); });

    QObject::connect(loopFx_ReptLen, &QComboBox::currentIndexChanged, this,
                     [this] { on_LoopFx_ComboBox_changed(loopFx_ReptLen, &BossRC::LoopFxCfg::RepeatLength); });

    QObject::connect(loopFx_Shift, &QComboBox::currentIndexChanged, this,
                     [this] { on_LoopFx_ComboBox_changed(loopFx_Shift, &BossRC::LoopFxCfg::ShiftShift); });

    QObject::connect(loopFx_Flick, &QSpinBox::valueChanged, this,
                     [this] { on_LoopFx_SpinBox_changed(loopFx_Flick, &BossRC::LoopFxCfg::VinylFlickFlick); });

    // Rhythm callback
    QObject::connect(rhythm_Level, &QSlider::valueChanged, this, [this] {
        on_Rhythm_Slider_changed(rhythm_Level, &BossRC::Rhythm::Level);
        label_rhythm_LevelValue->setText(std::to_string(rhythm_Level->value()).c_str());
    });

    QObject::connect(rhythm_Reverb, &QSlider::valueChanged, this, [this] {
        on_Rhythm_Slider_changed(rhythm_Reverb, &BossRC::Rhythm::Reverb);
        label_rhythm_ReverbValue->setText(std::to_string(rhythm_Reverb->value()).c_str());
    });

    QObject::connect(rhythm_Pattern, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_Pattern, &BossRC::Rhythm::Pattern); });

    QObject::connect(rhythm_Variation, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_Variation, &BossRC::Rhythm::Variation); });

    QObject::connect(rhythm_VarChange, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_VarChange, &BossRC::Rhythm::VariationChange); });

    QObject::connect(rhythm_Kit, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_Kit, &BossRC::Rhythm::Kit); });

    QObject::connect(rhythm_Beat, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_Beat, &BossRC::Rhythm::Beat); });

    QObject::connect(rhythm_Start, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_Start, &BossRC::Rhythm::Start); });

    QObject::connect(rhythm_Stop, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_Stop, &BossRC::Rhythm::Stop); });

    QObject::connect(rhythm_RecCount, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_RecCount, &BossRC::Rhythm::RecCount); });

    QObject::connect(rhythm_PlayCount, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_PlayCount, &BossRC::Rhythm::PlayCount); });

    QObject::connect(rhythm_Fill, &QCheckBox::checkStateChanged, this,
                     [this] { on_Rhythm_CheckBox_changed(rhythm_Fill, &BossRC::Rhythm::Fill); });

    QObject::connect(rhythm_Part1, &QCheckBox::checkStateChanged, this,
                     [this] { on_Rhythm_CheckBox_changed(rhythm_Part1, &BossRC::Rhythm::Part1); });
    QObject::connect(rhythm_Part2, &QCheckBox::checkStateChanged, this,
                     [this] { on_Rhythm_CheckBox_changed(rhythm_Part2, &BossRC::Rhythm::Part2); });
    QObject::connect(rhythm_Part3, &QCheckBox::checkStateChanged, this,
                     [this] { on_Rhythm_CheckBox_changed(rhythm_Part3, &BossRC::Rhythm::Part3); });
    QObject::connect(rhythm_Part4, &QCheckBox::checkStateChanged, this,
                     [this] { on_Rhythm_CheckBox_changed(rhythm_Part4, &BossRC::Rhythm::Part4); });

    QObject::connect(rhythm_ToneLow, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_ToneLow, &BossRC::Rhythm::ToneLow); });
    QObject::connect(rhythm_ToneHigh, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_ToneHigh, &BossRC::Rhythm::ToneHigh); });

    QObject::connect(rhythm_State, &QComboBox::currentIndexChanged, this,
                     [this] { on_Rhythm_ComboBox_changed(rhythm_State, &BossRC::Rhythm::State); });

    // Control callbacks
    auto ConnectComboBox_Control = [&](QComboBox* cb, auto member) {
        QObject::connect(cb, &QComboBox::currentIndexChanged, this, [this, cb, member] { on_Control_ComboBox_changed(cb, member); });
    };

    ConnectComboBox_Control(control_Pedal1, &BossRC::Ctl::Pedal1);
    ConnectComboBox_Control(control_Pedal2, &BossRC::Ctl::Pedal2);
    ConnectComboBox_Control(control_Pedal3, &BossRC::Ctl::Pedal3);
    ConnectComboBox_Control(control_Control1, &BossRC::Ctl::Ctl1);
    ConnectComboBox_Control(control_Control2, &BossRC::Ctl::Ctl2);
    ConnectComboBox_Control(control_Expression, &BossRC::Ctl::Exp);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_ToolMenu_New()
{
    _is_loading = true;

    try {
        setDirname("");
        _filename_mem = _filename_sys = "";

        _database_system = {};
        _database_memory = {};

        _stack.clear();

        cb_Memory->setCurrentIndex(0);
        load_memory(cb_Memory->currentIndex());

        const auto beat = rhythm_Beat->currentData().value<QPoint>();
        const auto loopFx_type = loopFx_Type->currentIndex();

        _boss_assign_dialog->load_assign(&_database_memory, cb_Memory->currentIndex(), beat, loopFx_type, _language);
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_ToolMenu_Open()
{
    _is_loading = true;

    try {
        if (const auto dirname = QFileDialog::getExistingDirectory(this, tr("Open a DATA directory"), "").toStdString(); !dirname.empty()) {
            setDirname(dirname);

            // We must read the newest files
            _filename_sys = GetOldestFile(dirname + "/SYSTEM1.RC0", dirname + "/SYSTEM2.RC0");
            _filename_mem = GetOldestFile(dirname + "/MEMORY1.RC0", dirname + "/MEMORY2.RC0");

            load_database_sys(_filename_sys);
            load_database_mem(_filename_mem);
            spdlog::info("SYSTEM: {} - {}", _database_system.database_counter, _filename_sys);
            spdlog::info("MEMORY: {} - {}", _database_memory.database_counter, _filename_mem);

            // Move to next version
            _filename_sys = (_filename_sys.find("SYSTEM1.RC0") != std::string::npos ? "SYSTEM2.RC0" : "SYSTEM1.RC0");
            _filename_mem = (_filename_mem.find("MEMORY1.RC0") != std::string::npos ? "MEMORY2.RC0" : "MEMORY1.RC0");

            _database_system.database_counter += 2;
            _database_memory.database_counter += 2;

            _stack.clear();

            cb_Memory->setCurrentIndex(0);
            load_memory(cb_Memory->currentIndex());

            const auto beat = rhythm_Beat->currentData().value<QPoint>();
            const auto loopFx_type = loopFx_Type->currentIndex();

            _boss_assign_dialog->load_assign(&_database_memory, cb_Memory->currentIndex(), beat, loopFx_type, _language);
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
        setDirname("");
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_ToolMenu_Save(const bool askDirname)
{
    try {
        if (_dirname.empty() || askDirname) {
            const auto dirname =
                QFileDialog::getExistingDirectory(this, tr("Save to MEMORY/SYSTEM files to a directory"), "").toStdString();

            if (dirname.empty()) {
                return;
            }

            setDirname(dirname);
        }

        if (const auto response = QMessageBox::question(nullptr, "Write changes to directory ?",
                                                        QString::fromStdString("Do you want to write to files into:\n" + _dirname));
            response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        _database_system.save(_dirname + "/" + _filename_sys);
        _database_memory.save(_dirname + "/" + _filename_mem);

        QMessageBox(QMessageBox::Information, "Information",
                    fmt::format("{} and {} files have been written.", _filename_sys, _filename_mem).c_str())
            .exec();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "Error on save", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_ToolMenu_PresetSave()
{
    try {
        const auto memory_index = cb_Memory->currentIndex();
        const auto current_name = _database_memory.memories[memory_index].name.name;

        if (const auto filename =
                QFileDialog::getSaveFileName(this, "Save a preset to file",
                                             (BossRc500::Resources::Presets().toStdString() + "/" + current_name + ".json").c_str(),
                                             "Preset files (*.json)")
                    .toStdString();
            !filename.empty()) {
            std::filesystem::path path(filename);
            if (!path.has_extension())
                path.replace_extension(".json");

            std::ofstream file(path);
            file << nlohmann::ordered_json(_database_memory.memories[memory_index]).dump(2) << std::endl;

            loadPresets();
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "Error on preset save", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_ToolMenu_PresetLoad(const QString& filename)
{
    _is_loading = true;

    try {
        std::ifstream file(filename.toStdString());

        nlohmann::json json;
        const auto memory_index = cb_Memory->currentIndex();
        file >> json;
        _database_memory.memories[memory_index] = json;

        load_memory(cb_Memory->currentIndex());

        // To be refactored... (duplicate code)
        const auto str = fmt::format("{} - {}", memory_index + 1, _database_memory.memories[memory_index].name.to_string(memory_index));
        cb_Memory->setItemText(memory_index, str.c_str());

        const auto beat = rhythm_Beat->currentData().value<QPoint>();
        const auto loopFx_type = loopFx_Type->currentIndex();

        _boss_assign_dialog->load_assign(&_database_memory, cb_Memory->currentIndex(), beat, loopFx_type, _language);
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "Error on preset load", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_ToolMenu_System()
{
    try {
        QDialog dialog;
        BossRc500SystemDialog systemDialog(dialog, _database_system, _language);
        dialog.setWindowTitle("RC-500 LOOP STATION - System");
        dialog.setModal(true);
        dialog.exec();
        if (systemDialog.apply) {
            _database_system = std::move(systemDialog.database);

            load_memory(cb_Memory->currentIndex());

            const auto beat = rhythm_Beat->currentData().value<QPoint>();
            const auto loopFx_type = loopFx_Type->currentIndex();

            _boss_assign_dialog->load_assign(&_database_memory, cb_Memory->currentIndex(), beat, loopFx_type, _language);
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_ToolMenu_Themes(const QString& filename)
{
    try {
        QFile file{filename};
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            std::cerr << "Failed to open theme file: " << filename.toStdString() << std::endl;
        }
        qApp->setStyleSheet(QTextStream(&file).readAll());
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_copy()
{
    try {
        int memory_slot = std::stoi(cb_Memory->currentText().toStdString());
        int copy_from_slot = std::stoi(cb_CopyFrom->currentText().toStdString());
        int copy_to_slot = std::stoi(cb_CopyTo->currentText().toStdString());

        if (copy_from_slot > copy_to_slot) {
            throw std::runtime_error("Copy From slot must be less than copy to slot");
        }

        if (const auto response = QMessageBox::question(
                nullptr, "Copy memories ?",
                fmt::format("Do you want to copy memory slot {}\ninto slots from {} to {}", memory_slot, copy_from_slot, copy_to_slot)
                    .c_str());
            response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        // Update database and write it to disk
        const auto& slot = _database_memory.memories[memory_slot - 1];

        for (int i = copy_from_slot; i <= copy_to_slot; ++i) {
            auto& mem = _database_memory.memories[i - 1];

            // Save values to be restored after the copy
            const auto backup = mem;

            // Copy all the values
            mem = slot;

            // Restore some values from the original
            mem.name = backup.name;
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_advanced_copy()
{
    try {
        QDialog dialog(this);
        dialog.setWindowTitle("RC-500 LOOP STATION - Memory Copy");

        const auto* copyDialog = new BossRc500AdvancedCopyDialog{dialog, _database_memory, cb_Memory->currentIndex()};
        copyDialog->cb_CopyFrom->setCurrentIndex(cb_CopyFrom->currentIndex());
        copyDialog->cb_CopyTo->setCurrentIndex(cb_CopyTo->currentIndex());

        // dialog.setModal(true);
        dialog.exec();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_edit()
{
    try {
        const int memory_index = cb_Memory->currentIndex();

        bool ok = false;
        auto text = QInputDialog::getText(nullptr, "Memory Name", "Memory Name", QLineEdit::Normal,
                                          _database_memory.memories[memory_index].name.to_string(memory_index).c_str(), &ok)
                        .toStdString();
        if (ok) {
            _database_memory.memories[memory_index].name.name = text;

            const auto name = fmt::format("{} - {}", memory_index + 1, text);
            cb_Memory->setItemText(memory_index, name.c_str());
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_control_pdlctl_help()
{
    try {
        QDialog dialog;
        Ui::DialogText textDialog;
        textDialog.setupUi(&dialog);

        BossRc500::Tooltips tooltips;
        textDialog.textEdit->setHtml(tooltips.control_PdlCtl());

        dialog.setWindowTitle("RC-500 LOOP STATION - Assign Pdl Ctl Help");
        dialog.setModal(true);
        dialog.exec();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "Error on PdlCtl help", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_memory_previous() const
{
    if (const int memory_index = cb_Memory->currentIndex(); memory_index > 0) {
        cb_Memory->setCurrentIndex(memory_index - 1);
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_memory_next() const
{
    if (const int memory_index = cb_Memory->currentIndex(); memory_index < 98) {
        cb_Memory->setCurrentIndex(memory_index + 1);
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_memory_changed()
{
    _is_loading = true;

    try {
        load_memory(cb_Memory->currentIndex());

        const auto beat = rhythm_Beat->currentData().value<QPoint>();
        const auto loopFx_type = loopFx_Type->currentIndex();

        _boss_assign_dialog->load_assign(&_database_memory, cb_Memory->currentIndex(), beat, loopFx_type, _language);
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_rhythm_play()
{
    std::string drumkit_filename = BossRc500::Resources::DrumKits().toStdString();
    drumkit_filename += "/" + rhythm_Pattern->currentText().toStdString() + "_";
    drumkit_filename += rhythm_Variation->currentText().toStdString() + "_";

    auto beat = rhythm_Beat->currentText().toStdString();
    std::erase(beat, '/');

    drumkit_filename += beat + ".wav";

    // ma_decoder decoder;
    // if (const ma_result result = ma_decoder_init_file(drumkit_filename.c_str(), nullptr, &decoder); result != MA_SUCCESS) {
    //     spdlog::error("Failed to open file: {}", drumkit_filename);
    //     return;
    // }
    //
    // /*
    // A decoder is a data source which means we just use ma_data_source_set_looping() to set the
    // looping state. We will read data using ma_data_source_read_pcm_frames() in the data callback.
    // */
    // ma_data_source_set_looping(&decoder, MA_TRUE);
    //
    // ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    // deviceConfig.playback.format = decoder.outputFormat;
    // deviceConfig.playback.channels = decoder.outputChannels;
    // deviceConfig.sampleRate = decoder.outputSampleRate;
    // deviceConfig.dataCallback = data_callback;
    // deviceConfig.pUserData = &decoder;
    //
    // ma_device device;
    // if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
    //     spdlog::error("Failed to open playback device.");
    //     ma_decoder_uninit(&decoder);
    //     return;
    // }
    //
    // if (ma_device_start(&device) != MA_SUCCESS) {
    //     spdlog::error("Failed to start playback device.");
    //     ma_device_uninit(&device);
    //     ma_decoder_uninit(&decoder);
    //     return;
    // }
    //
    // QMessageBox(QMessageBox::Information, "Information", drumkit_filename.c_str()).exec();
    //
    // ma_device_uninit(&device);
    // ma_decoder_uninit(&decoder);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::load_database_mem(const std::string& filename)
{
    _database_memory = BossRC::MemoryFile::load(filename);

    // Add name to memory index
    for (int i = 1; i <= 99; ++i) {
        cb_Memory->setItemText(i - 1, fmt::format("{} - {}", i, _database_memory.memories[i - 1].name.to_string(i)).c_str());
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::load_database_sys(const std::string& filename)
{
    _database_system = BossRC::SystemFile::load(filename);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::load_memory(const int memory_index)
{
    {
        // TRACK 1
        const auto& track1 = _database_memory.memories[memory_index].tracks[0];

        track1_Reverse->setChecked(track1.Rev);
        track1_Level->setValue(track1.PlyLvl);
        track1_Pan->setCurrentIndex(track1.Pan);
        track1_OneShot->setChecked(track1.One);
        track1_LoopFx->setChecked(track1.LoopFx);
        track1_Start->setCurrentIndex(track1.StrtMod);
        track1_Stop->setCurrentIndex(track1.StpMod);
        track1_Measure->setCurrentIndex(track1.Measure);
        track1_LoopSync->setChecked(track1.LoopSync);
        track1_TempoSync->setChecked(track1.TempoSync);
        track1_Input->setCurrentIndex(track1.Input);
        track1_Output->setCurrentIndex(track1.Output);
    }

    {
        // TRACK 2
        const auto& track2 = _database_memory.memories[memory_index].tracks[1];

        track2_Reverse->setChecked(track2.Rev);
        track2_Level->setValue(track2.PlyLvl);
        track2_Pan->setCurrentIndex(track2.Pan);
        track2_OneShot->setChecked(track2.One);
        track2_LoopFx->setChecked(track2.LoopFx);
        track2_Start->setCurrentIndex(track2.StrtMod);
        track2_Stop->setCurrentIndex(track2.StpMod);
        track2_Measure->setCurrentIndex(track2.Measure);
        track2_LoopSync->setChecked(track2.LoopSync);
        track2_TempoSync->setChecked(track2.TempoSync);
        track2_Input->setCurrentIndex(track2.Input);
        track2_Output->setCurrentIndex(track2.Output);
    }

    // MASTER (REC/PLAY)
    {
        const auto& master = _database_memory.memories[memory_index].master;
        record_DubMode->setCurrentIndex(master.DubMode);

        // RECORD
        record_DubMode->setCurrentIndex(master.DubMode);
        record_RecordAction->setCurrentIndex(master.RecAction);

        int recQuantize = master.RecQuantize;
        if (recQuantize == 2)
            recQuantize = 1;
        record_Quantize->setCurrentIndex(recQuantize);

        record_AutoRecord->setChecked(master.AutoRec);
        record_AutoRecordSensitivity->setValue(master.AutoRecSens);
        record_AutoRecordSource->setCurrentIndex(master.AutoRecSrc);
        record_LoopLength->setCurrentIndex(master.LpLen);

        // PLAY
        play_Tempo->setValue(master.Tempo / 10.0);
        play_PlayMode->setCurrentIndex(master.PlayMode);
        play_SingleChange->setCurrentIndex(master.SinglPlayeChange);
        play_Level->setValue(master.Level);
        play_FadeTime->setCurrentIndex(master.FadeTime);
        play_AllStart->setCurrentIndex(master.AllStart);
        play_TrackChain->setCurrentIndex(master.TrackChain);
    }

    // LOOPFX
    {
        const auto& loopfx = _database_memory.memories[memory_index].loopFx;
        loopFx_Sw->setChecked(loopfx.Sw);
        loopFx_Type->setCurrentIndex(loopfx.FxType);

        // These one are reverted from 8 to 1
        const auto scatter = loopfx.ScatterLength;
        loopFx_ScatLen->setCurrentIndex(scatter ? 9 - scatter : 0);

        const auto repeat = loopfx.RepeatLength;
        loopFx_ReptLen->setCurrentIndex(repeat ? 9 - repeat : 0);

        // Index 1 is not available (0, 2, 3, 4, ....)
        const auto shift = loopfx.ShiftShift;
        const auto found_Shift = loopFx_Shift->findData(shift);
        loopFx_Shift->setCurrentIndex(found_Shift != -1 ? found_Shift : 2);
        // spdlog::info("Found shift value: {} at index: ", shift, found_Shift);

        loopFx_Flick->setValue(loopfx.VinylFlickFlick);
    }

    // RHYTHM
    {
        const auto& rhythm = _database_memory.memories[memory_index].rhythm;
        // spdlog::info("{}", rhythm.dump(2));

        rhythm_Level->setValue(rhythm.Level);
        rhythm_Reverb->setValue(rhythm.Reverb);

        // Beat before Pattern to drive Pattern combobox items
        rhythm_Beat->setCurrentIndex(rhythm.Beat);

        rhythm_Pattern->setCurrentIndex(rhythm.Pattern);
        rhythm_Variation->setCurrentIndex(rhythm.Variation);
        rhythm_VarChange->setCurrentIndex(rhythm.VariationChange);
        rhythm_Kit->setCurrentIndex(rhythm.Kit);
        rhythm_Start->setCurrentIndex(rhythm.Start);
        rhythm_Stop->setCurrentIndex(rhythm.Stop);
        rhythm_RecCount->setCurrentIndex(rhythm.RecCount);
        rhythm_PlayCount->setCurrentIndex(rhythm.PlayCount);
        rhythm_Fill->setChecked(rhythm.Fill);
        rhythm_Part1->setChecked(rhythm.Part1);
        rhythm_Part2->setChecked(rhythm.Part2);
        rhythm_Part3->setChecked(rhythm.Part3);
        rhythm_Part4->setChecked(rhythm.Part4);
        rhythm_ToneLow->setCurrentIndex(rhythm.ToneLow);
        rhythm_ToneHigh->setCurrentIndex(rhythm.ToneHigh);
        rhythm_State->setCurrentIndex(rhythm.State);
    }

    // CONTROL
    // These controls are effective only if the associated PREF is set to MEMORY
    {
        auto& mem_ctl = _database_memory.memories[memory_index].ctl;
        auto& sys_pref = _database_system.pref;
        auto& sys_ctl = _database_system.ctl;

        auto SetControl = [&](QComboBox* cb, auto pref_name, auto ctl_name) {
            auto sys_pref_value = sys_pref.*pref_name;
            auto sys_ctl_value = sys_ctl.*ctl_name;

            auto mem_ctl_value = mem_ctl.*ctl_name;

            cb->setEnabled(sys_pref_value == 0);
            cb->setCurrentIndex(cb->isEnabled() ? mem_ctl_value : sys_ctl_value);
        };

        SetControl(control_Pedal1, &BossRC::Pref::Pdl1, &BossRC::Ctl::Pedal1);
        SetControl(control_Pedal2, &BossRC::Pref::Pdl2, &BossRC::Ctl::Pedal2);
        SetControl(control_Pedal3, &BossRC::Pref::Pdl3, &BossRC::Ctl::Pedal3);
        SetControl(control_Control1, &BossRC::Pref::Ctl1, &BossRC::Ctl::Ctl1);
        SetControl(control_Control2, &BossRC::Pref::Ctl2, &BossRC::Ctl::Ctl2);
        SetControl(control_Expression, &BossRC::Pref::Exp, &BossRC::Ctl::Exp);
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Level_changed(QSlider* slider)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(slider), &BossRC::Track::PlyLvl, slider->value(), slider);

    const auto value_str = std::to_string(slider->value());

    (slider == track1_Level ? label_track1_PlyLevel : label_track2_PlyLevel)->setText(value_str.c_str());
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Pan_changed(QComboBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::Pan, cb->currentIndex(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Start_changed(QComboBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::StrtMod, cb->currentIndex(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Stop_changed(QComboBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::StpMod, cb->currentIndex(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Measure_changed(QComboBox* cb)
{
    const int memory_index = cb_Memory->currentIndex();
    const int track_index = getTrackId(cb);
    int value = cb->currentIndex();
    spdlog::info("Memory: {}, Track: {}, Measure: ", (memory_index + 1), (track_index + 1), value);

    int measMod = 0, measLen = 0, measBtLp = 0;
    if (value == 0) {
        // AUTO
        measMod = measLen = measBtLp = 0;
    }
    else if (value == 1) {
        // FREE
        measMod = 1;
        measLen = measBtLp = 0;
    }
    else if (value >= 2 && value <= 7) {
        // NOTE mode
        measMod = 2;
        measLen = 1;
        measBtLp = value - 1;
    }
    else {
        // MEASURE mode
        measMod = 2;
        measLen = value - 7;
        measBtLp = 0;
    }

    update_mem_track_database2(memory_index, getTrackId(cb), &BossRC::Track::Measure, value, cb);

    _database_memory.memories[memory_index].tracks[track_index].MeasMod = measMod;
    _database_memory.memories[memory_index].tracks[track_index].MeasLen = measLen;
    _database_memory.memories[memory_index].tracks[track_index].MeasBtLp = measBtLp;
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Reverse_changed(QCheckBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::Rev, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_LoopFx_changed(QCheckBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::LoopFx, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_OneShot_changed(QCheckBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::One, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_LoopSync_changed(QCheckBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::LoopSync, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_TempoSync_changed(QCheckBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::TempoSync, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Input_changed(QComboBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::Input, cb->currentIndex(), cb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_Output_changed(QComboBox* cb)
{
    update_mem_track_database2(cb_Memory->currentIndex(), getTrackId(cb), &BossRC::Track::Output, cb->currentIndex(), cb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Master_ComboBox_changed(QComboBox* cb, Member member)
{
    // Some ugly ack for Quantize
    // NOTE: may be a usage for userData and QVariant here, but too long to refactor
    int value = cb->currentIndex();

    if (cb == record_Quantize) {
        if (value == 1) {
            // MEASURE
            value = 2;
        }
    }

    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::master, member, value, cb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Master_CheckBox_changed(QCheckBox* cb, Member member)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::master, member, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Master_SpinBox_changed(QSpinBox* sb, Member member)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::master, member, sb->value(), sb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Master_DoubleSpinBox_changed(QDoubleSpinBox* sb, Member member, int factor)
{
    auto value = sb->value();
    if (sb == play_Tempo)
        value *= 10;

    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::master, member, value, sb);
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::on_LoopFx_FxType_changed()
{
    on_LoopFx_ComboBox_changed(loopFx_Type, &BossRC::LoopFxCfg::FxType);

    // Discard some combobox
    const auto loopFxType_index = loopFx_Type->currentIndex();
    loopFx_ScatLen->setEnabled(BossRc500::IsScatter(loopFxType_index));
    loopFx_ReptLen->setEnabled(BossRc500::IsRepeat(loopFxType_index));
    loopFx_Shift->setEnabled(BossRc500::IsShift(loopFxType_index));
    loopFx_Flick->setEnabled(BossRc500::IsVinyl(loopFxType_index));
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_LoopFx_ComboBox_changed(QComboBox* cb, Member member)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::loopFx, member, cb->currentData().value<int>(), cb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_LoopFx_CheckBox_changed(QCheckBox* cb, Member member)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::loopFx, member, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_LoopFx_SpinBox_changed(QSpinBox* sb, Member member)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::loopFx, member, sb->value(), sb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Rhythm_ComboBox_changed(QComboBox* cb, Member name)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::rhythm, name, cb->currentIndex(), cb);

    if (cb == rhythm_Beat) {
        const auto beat = cb->currentData().value<QPoint>();

        // On beat change, we need to recompute the available values for Scat, Rep and Shift
        spdlog::info("Beat signature: {}/{}", beat.x(), beat.y());
        BossRc500::ScatLen_EnableItems(loopFx_ScatLen, beat);
        BossRc500::ReptLen_EnableItems(loopFx_ReptLen, beat);
        BossRc500::ShiftLen_EnableItems(loopFx_Shift, beat);

        const auto& rhythm = _database_memory.memories[cb_Memory->currentIndex()].rhythm;
        const auto pattern = rhythm.Pattern;

        BossRc500::RhythmPatternWithBeat(rhythm_Pattern, beat, pattern);
    }
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Rhythm_CheckBox_changed(QCheckBox* cb, Member name)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::rhythm, name, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Rhythm_Slider_changed(QSlider* s, Member name)
{
    update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::rhythm, name, s->value(), s);
}

// --------------------------------------------------------------------------
template <typename Member>
void BossRc500MainWindow::on_Control_ComboBox_changed(QComboBox* cb, Member member)
{
    // Change the database only if cb is enabled // MEMORY mode in sys.PREF
    if (cb->isEnabled()) {
        update_mem_database(cb_Memory->currentIndex(), &BossRC::Memory::ctl, member, cb->currentIndex(), cb);
    }
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::setDirname(const std::string& dirname)
{
    _dirname = dirname;

    const std::string title = "RC-500 LOOP STATION - " + (_dirname.empty() ? "[Untitled]" : _dirname);
    setWindowTitle(title.c_str());
}

// --------------------------------------------------------------------------
void BossRc500MainWindow::loadPresets()
{
    _presetLoadMenu->clear();

    for (auto&& file_info : QDir{BossRc500::Resources::Presets()}.entryInfoList(QDir::Files)) {
        if (file_info.completeSuffix() == "json") {
            auto filename = file_info.absoluteFilePath();
            spdlog::info("Add preset file [{}]", filename.toStdString());

            _presetLoadMenu->addAction(file_info.baseName(), [this, filename] { on_ToolMenu_PresetLoad(filename); });
        }
    }
}

// ---------------------------------------------------------------------------------------------------
// Check the version of the file (last 4bytes) and return the newest one, with the next counter to use
// ---------------------------------------------------------------------------------------------------
std::string BossRc500MainWindow::GetOldestFile(const std::string& file1, const std::string& file2)
{
    if (!std::filesystem::exists(file1) || !std::filesystem::exists(file2)) {
        throw std::runtime_error("Data directory must contains two RC0 files : " + file1 + " and " + file2 + ".");
    }
    const auto counter1 = BossRC::readVersion(file1);
    const auto counter2 = BossRC::readVersion(file2);

    return counter1 > counter2 ? file1 : file2;
}
