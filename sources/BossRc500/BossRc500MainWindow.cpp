#include "BossRc500MainWindow.hpp"
#include "BossRc500AssignDialog.hpp"
#include "BossRc500SystemDialog.hpp"
#include "BossRc500AdvancedCopyDialog.hpp"
#include "BossRc500.hpp"
#include "Designer/ui_Boss-rc500-text.h"

#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QInputDialog>
#include <QMenu>
#include <QActionGroup>
#include <QDirIterator>
#include <QUndoView>

#include <miniaudio/miniaudio.h>

#include <iostream>
#include <filesystem>
#include <algorithm>

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
BossRc500MainWindow::BossRc500MainWindow()
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::setup()
{
    setupUi(this);

    // Add some tweaks...
    setFixedSize(width(), height());

    auto fileMenu = new QMenu("File", menuBar());
    fileMenu->addAction("New", QKeySequence::New,          this, &BossRc500MainWindow::on_ToolMenu_New);
    fileMenu->addAction("Open...", QKeySequence::Open,     this, &BossRc500MainWindow::on_ToolMenu_Open);
    fileMenu->addSeparator();
    fileMenu->addAction("Save", QKeySequence::Save,        this, &BossRc500MainWindow::on_ToolMenu_Save);
    fileMenu->addAction("Save as...", QKeySequence::SaveAs, [this] { on_ToolMenu_Save(true); });

    fileMenu->addSeparator();
    fileMenu->addAction("Save as preset...", [this] { on_ToolMenu_PresetSave(); });

    _presetLoadMenu = new QMenu("Load Preset", fileMenu);
    loadPresets();
    fileMenu->addMenu(_presetLoadMenu);

    fileMenu->addSeparator();
    fileMenu->addAction("Assign...",   this, &BossRc500MainWindow::on_ToolMenu_Assign);
    fileMenu->addSeparator();
    fileMenu->addAction("System...",   this, &BossRc500MainWindow::on_ToolMenu_System);
    fileMenu->addSeparator();

    {
        _preferences_themes_group = new QActionGroup(this);
        _preferences_themes_group->isExclusive();

        // Add the default theme (Default.css)
        auto themesMenu = new QMenu("Themes", fileMenu);

        auto default_themes = themesMenu->addAction("Default", [this] { on_ToolMenu_Themes(BossRc500::Resources::Themes() + "/Default.css"); } );
        default_themes->setCheckable(true);
        default_themes->setChecked(true);
        _preferences_themes_group->addAction(default_themes);

        for (auto&& file_info : QDir{BossRc500::Resources::Themes()}.entryInfoList(QDir::Files)) {
            std::cout << "Loading theme file [" << file_info.absoluteFilePath().toStdString() << "]" << std::endl;

            if (file_info.completeSuffix() == "css" && file_info.baseName() != "Default") {
                auto filename = file_info.absoluteFilePath();
                std::cout << "Add theme file [" << filename.toStdString() << "]" << std::endl;

                auto theme = themesMenu->addAction(file_info.baseName(), [this, filename] { on_ToolMenu_Themes(filename); });
                theme->setCheckable(true);
                _preferences_themes_group->addAction(theme);
            }
        }

        fileMenu->addMenu(themesMenu);
    }

    {
        _preferences_language_group = new QActionGroup(this);
        _preferences_language_group->isExclusive();

        auto preferencesMenu = new QMenu("Preferences", fileMenu);
        auto default_english = preferencesMenu->addAction("Default", [this] { _language = ""; add_tooltips(); } );
        default_english->setCheckable(true);
        default_english->setChecked(true);
        _preferences_language_group->addAction(default_english);

        for (auto&& l : BossRc500::Configuration::Instance().languages) {
            auto lang = preferencesMenu->addAction(l.first, [this, code = l.second] {
                _language = code;
                add_tooltips();
            } );
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

    auto editMenu = new QMenu("Edit", menuBar());
    editMenu->addAction("Undo", QKeySequence::Undo, [this] { _stack.undo(); });
    editMenu->addAction("Redo", QKeySequence::Redo, [this] { _stack.redo(); });
    fileMenu->addSeparator();
    editMenu->addAction("Show/Hide Stack",
            [this] {
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
        _stack_dialog = new QDialog(this);
        _stack_dialog->setLayout(new QGridLayout);

        auto view = new QUndoView(_stack_dialog);
        view->setStack(&_stack);
        _stack_dialog->layout()->addWidget(view);

        _stack_dialog->setMinimumSize(250, 400);
    }

    // Start with a default MEMORY file
    on_ToolMenu_New();

    // No more drumkits...
    button_rhythm_Play->hide();
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::add_tooltips()
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

#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::add_combo_items()
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
void
BossRc500MainWindow::add_callbacks()
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

    QObject::connect(rhythm_State, &QComboBox::currentIndexChanged,
            this, [this] { on_Rhythm_ComboBox_changed(rhythm_State, "State"); });

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
BossRc500MainWindow::on_ToolMenu_New()
{
    _is_loading = true;

    try {
        setDirname("");
        load_database_sys(BossRc500::Resources::Templates().toStdString() + "/SYSTEM_DEFAULT.RC0");
        load_database_mem(BossRc500::Resources::Templates().toStdString() + "/MEMORY_DEFAULT.RC0");

        _stack.clear();

        cb_Memory->setCurrentIndex(0);
        load_memory(cb_Memory->currentIndex());

    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_ToolMenu_Open()
{
    _is_loading = true;

    try {
        auto dirname = QFileDialog::getExistingDirectory(this,
                tr("Open a DATA directory"), "").toStdString();
        if (!dirname.empty()) {

            setDirname(dirname);

            if (auto filename_sys = dirname + "/SYSTEM1.RC0"; std::filesystem::exists(filename_sys)) {
                load_database_sys(filename_sys);
            }

            if (auto filename_nem = dirname + "/MEMORY1.RC0"; std::filesystem::exists(filename_nem)) {
                load_database_mem(filename_nem);

                _stack.clear();

                cb_Memory->setCurrentIndex(0);
                load_memory(cb_Memory->currentIndex());
            }
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
        setDirname("");
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_ToolMenu_Save(bool askDirname)
{
    try {
        if (_dirname.empty() || askDirname) {
            auto dirname = QFileDialog::getExistingDirectory(this,
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
BossRc500MainWindow::on_ToolMenu_PresetSave()
{
    try {
        auto memory_index = cb_Memory->currentIndex();
        auto current_name = _database_mem["mem"][memory_index]["name"].get<std::string>();

        if (auto filename = QFileDialog::getSaveFileName(this, "Save a preset to file",
                (BossRc500::Resources::Presets().toStdString() + "/" + current_name + ".json").c_str(),
                "Preset files (*.json)").toStdString(); !filename.empty()) {

            std::filesystem::path path(filename);
            if (!path.has_extension()) path.replace_extension(".json");

            std::ofstream file(path);
            file << _database_mem["mem"][memory_index].dump(2) << std::endl;

            loadPresets();
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_ToolMenu_PresetLoad(const QString& filename)
{
    _is_loading = true;

    try {
        std::ifstream file(filename.toStdString());

        auto memory_index = cb_Memory->currentIndex();
        file >> _database_mem["mem"][memory_index];
        load_memory(cb_Memory->currentIndex());

        // To be refactored... (duplicate code)
        auto index = std::to_string(memory_index + 1);
        if (auto found_name = _database_mem["mem"][memory_index].find("name");
                found_name != _database_mem["mem"][memory_index].end()) {
            index += " - " + found_name->get<std::string>();
        }
        cb_Memory->setItemText(memory_index, index.c_str());
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_ToolMenu_Assign()
{
    try {
        auto beat = rhythm_Beat->currentData().value<QPoint>();
        auto loopFx_type = loopFx_Type->currentIndex();

        QDialog dialog;
        BossRc500AssignDialog assignDialog(dialog,
                _database_mem,
                cb_Memory->currentIndex(),
                beat,
                loopFx_type,
                _language);
        dialog.setWindowTitle("RC-500 LOOP STATION - Assign");
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
BossRc500MainWindow::on_ToolMenu_System()
{
    try {
        QDialog dialog;
        BossRc500SystemDialog systemDialog(dialog, _database_sys, _language);
        dialog.setWindowTitle("RC-500 LOOP STATION - System");
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
BossRc500MainWindow::on_ToolMenu_Themes(const QString& filename)
{
    try {
        QFile file{filename};
        file.open(QFile::ReadOnly | QFile::Text);
        qApp->setStyleSheet(QTextStream(&file).readAll());
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_copy()
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
            auto& mem = _database_mem["mem"][i - 1];

            // Save values to be restored after the copy
            auto backup = mem;

            // Copy all the values
            mem = slot;

            // Restore some values from the original
            mem["id"] = backup["id"];
            mem["NAME"] = backup["NAME"];
            mem["name"] = backup["name"];
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_advanced_copy()
{
    try {
        QDialog dialog;
        BossRc500AdvancedCopyDialog copyDialog{dialog, _database_mem, cb_Memory->currentIndex()};
        copyDialog.cb_CopyFrom->setCurrentIndex(cb_CopyFrom->currentIndex());
        copyDialog.cb_CopyTo->setCurrentIndex(cb_CopyTo->currentIndex());

        dialog.setWindowTitle("RC-500 LOOP STATION - Memory Copy");
        dialog.setModal(true);
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
BossRc500MainWindow::on_control_pdlctl_help()
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
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_memory_previous()
{
    int memory_index = cb_Memory->currentIndex();
    if (memory_index > 0) {
        cb_Memory->setCurrentIndex(memory_index - 1);
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_memory_next()
{
    int memory_index = cb_Memory->currentIndex();
    if (memory_index < 98) {
        cb_Memory->setCurrentIndex(memory_index + 1);
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_memory_changed()
{
    _is_loading = true;

    try {
        load_memory(cb_Memory->currentIndex());
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_rhythm_play()
{
    std::string drumkit_filename = BossRc500::Resources::DrumKits().toStdString();
    drumkit_filename += "/" + rhythm_Pattern->currentText().toStdString() + "_";
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
BossRc500MainWindow::load_database_mem(const std::string& filename)
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
BossRc500MainWindow::load_database_sys(const std::string& filename)
{
    _database_sys = ReadSystemDatabase(filename);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::load_memory(int memory_index)
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
        //std::cout << rhythm.dump(2) << std::endl;

        rhythm_Level->setValue(rhythm["Level"].get<int>());
        rhythm_Reverb->setValue(rhythm["Reverb"].get<int>());

        // Beat before Pattern to drive Pattern combobox items
        rhythm_Beat->setCurrentIndex(rhythm["Beat"].get<int>());

        rhythm_Pattern->setCurrentIndex(rhythm["Pattern"].get<int>());
        rhythm_Variation->setCurrentIndex(rhythm["Variation"].get<int>());
        rhythm_VarChange->setCurrentIndex(rhythm["VariationChange"].get<int>());
        rhythm_Kit->setCurrentIndex(rhythm["Kit"].get<int>());
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
        rhythm_State->setCurrentIndex(rhythm["State"].get<int>());
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
BossRc500MainWindow::on_Level_changed(QSlider* slider)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (slider->parentWidget() == gp_Track1 ? 0 : 1),
            "PlyLvl",
            slider->value(),
            slider);

    auto value_str = std::to_string(slider->value());

    (slider == track1_Level ? label_track1_PlyLevel : label_track2_PlyLevel)->setText(value_str.c_str());
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Pan_changed(QComboBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "Pan",
            cb->currentIndex(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Start_changed(QComboBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "StrtMod",
            cb->currentIndex(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Stop_changed(QComboBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "StpMod",
            cb->currentIndex(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Measure_changed(QComboBox* cb)
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

    update_mem_track_database(memory_index,
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "Measure",
            value,
            cb);

    _database_mem["mem"][memory_index]["TRACK"][track_index]["MeasMod"] = measMod;
    _database_mem["mem"][memory_index]["TRACK"][track_index]["MeasLen"] = measLen;
    _database_mem["mem"][memory_index]["TRACK"][track_index]["MeasBtLp"] = measBtLp;
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Reverse_changed(QCheckBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "Rev",
            cb->isChecked(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_LoopFx_changed(QCheckBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "LoopFx",
            cb->isChecked(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_OneShot_changed(QCheckBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "One",
            cb->isChecked(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_LoopSync_changed(QCheckBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "LoopSync",
            cb->isChecked(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_TempoSync_changed(QCheckBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "TempoSync",
            cb->isChecked(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Input_changed(QComboBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "Input",
            cb->currentIndex(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Output_changed(QComboBox* cb)
{
    update_mem_track_database(cb_Memory->currentIndex(),
            (cb->parentWidget() == gp_Track1 ? 0 : 1),
            "Output",
            cb->currentIndex(),
            cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Master_ComboBox_changed(QComboBox* cb, const char* name)
{
    // Some ugly ack for Quantize
    // NOTE: may be a usage for userData and QVariant here, but too long to refactor
    int value = cb->currentIndex();

    if (cb == record_Quantize) {
        if (value == 1) { // MEASURE
            value = 2;
        }
    }

    update_mem_database(cb_Memory->currentIndex(), "MASTER", name, value, cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Master_CheckBox_changed(QCheckBox* cb, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "MASTER", name, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Master_SpinBox_changed(QSpinBox* sb, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "MASTER", name, sb->value(), sb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Master_DoubleSpinBox_changed(QDoubleSpinBox* sb, const char* name, int factor)
{
    auto value = sb->value();
    if (sb == play_Tempo) value *= 10;

    update_mem_database(cb_Memory->currentIndex(), "MASTER", name, value, sb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_LoopFx_FxType_changed()
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
BossRc500MainWindow::on_LoopFx_ComboBox_changed(QComboBox* cb, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "LOOPFX", name,  cb->currentData().value<int>(), cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_LoopFx_CheckBox_changed(QCheckBox* cb, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "LOOPFX", name, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_LoopFx_SpinBox_changed(QSpinBox* sb, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "LOOPFX", name, sb->value(), sb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Rhythm_ComboBox_changed(QComboBox* cb, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "RHYTHM", name, cb->currentIndex(), cb);

    if (cb == rhythm_Beat) {
        auto beat = cb->currentData().value<QPoint>();

        // On beat change, we need to recompute the available values for Scat, Rep and Shift
        std::cout << "Beat signature: " << beat.x() << "/" << beat.y() << std::endl;
        BossRc500::ScatLen_EnableItems(loopFx_ScatLen, beat);
        BossRc500::ReptLen_EnableItems(loopFx_ReptLen, beat);
        BossRc500::ShiftLen_EnableItems(loopFx_Shift, beat);

        auto& rhythm = _database_mem["mem"][cb_Memory->currentIndex()]["RHYTHM"];
        auto pattern = rhythm["Pattern"].get<int>();

        BossRc500::RhythmPatternWithBeat(rhythm_Pattern, beat, pattern);
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Rhythm_CheckBox_changed(QCheckBox* cb, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "RHYTHM", name, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Rhythm_Slider_changed(QSlider* s, const char* name)
{
    update_mem_database(cb_Memory->currentIndex(), "RHYTHM", name, s->value(), s);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::on_Control_ComboBox_changed(QComboBox* cb, const char* name)
{
    // Change the database only if cb is enabled // MEMORY mode in sys.PREF
    if (cb->isEnabled()) {
        update_mem_database(cb_Memory->currentIndex(), "CTL", name, cb->currentIndex(), cb);
    }
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::setDirname(const std::string& dirname)
{
    _dirname = dirname;

    std::string title = "RC-500 LOOP STATION - " + (_dirname.empty() ? "[Untitled]" : _dirname);
    setWindowTitle(title.c_str());
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::loadPresets()
{
    _presetLoadMenu->clear();

    for (auto&& file_info : QDir{BossRc500::Resources::Presets()}.entryInfoList(QDir::Files)) {
        if (file_info.completeSuffix() == "json") {

            auto filename = file_info.absoluteFilePath();
            std::cout << "Add preset file [" << filename.toStdString() << "]" << std::endl;

            _presetLoadMenu->addAction(file_info.baseName(),
                    [this, filename] { on_ToolMenu_PresetLoad(filename); });
        }
    }

}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::WriteMemoryDatabase(const nlohmann::json& database, const std::string& filename)
{
    auto model = (BossRc500::Resources::Templates() + "/MEMORY.txt").toStdString();
    std::cout << "Using " << model << " as template file for MEMORY" << std::endl;
    WriteDatabase(database, filename, model);
}

// --------------------------------------------------------------------------
void
BossRc500MainWindow::WriteSystemDatabase(const nlohmann::json& database, const std::string& filename)
{
    auto model = (BossRc500::Resources::Templates() + "/SYSTEM.txt").toStdString();
    std::cout << "Using " << model << " as template file for SYSTEM" << std::endl;
    WriteDatabase(database, filename, model);
}
