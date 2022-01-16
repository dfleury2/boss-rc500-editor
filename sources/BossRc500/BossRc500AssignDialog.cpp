#include "BossRc500AssignDialog.hpp"
#include "Designer/ui_Boss-rc500-text.h"
#include "BossRc500.hpp"

#include <QStyleFactory>
#include <QMessageBox>

#include <iostream>

// --------------------------------------------------------------------------
BossRc500AssignDialog::BossRc500AssignDialog(QDialog& dialog,
        nlohmann::json db,
        int mem_index,
        const BossRc500::Beat& beat,
        int loopFxType) :
        _parent(dialog), database(std::move(db)), _memory_index(mem_index),
        _beat(beat), _loopfxType(loopFxType)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    _parent.setFixedSize(_parent.width(), _parent.height());

    _font_bold = _parent.font();
    _font_bold.setWeight(QFont::Weight::Bold);

    _assigns = { assign1_Assign, assign2_Assign, assign3_Assign, assign4_Assign,
            assign5_Assign, assign6_Assign, assign7_Assign, assign8_Assign };

    _sources = { assign1_Source, assign2_Source, assign3_Source, assign4_Source,
            assign5_Source, assign6_Source, assign7_Source, assign8_Source };

    _sourceModes = { assign1_SourceMode, assign2_SourceMode, assign3_SourceMode, assign4_SourceMode,
            assign5_SourceMode, assign6_SourceMode, assign7_SourceMode, assign8_SourceMode };

    _targets = { assign1_Target, assign2_Target, assign3_Target, assign4_Target,
                 assign5_Target, assign6_Target, assign7_Target, assign8_Target };

    _targetMins = { assign1_TargetMin, assign2_TargetMin, assign3_TargetMin, assign4_TargetMin,
                 assign5_TargetMin, assign6_TargetMin, assign7_TargetMin, assign8_TargetMin };

    _targetMaxs = { assign1_TargetMax, assign2_TargetMax, assign3_TargetMax, assign4_TargetMax,
                 assign5_TargetMax, assign6_TargetMax, assign7_TargetMax, assign8_TargetMax };

    _is_loading = true;


    try {
        add_tooltips();
        add_combo_items();
        add_callbacks();

        load_assign();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::add_tooltips()
{
// Add tooltips
#if QT_CONFIG(tooltip)
    BossRc500::Tooltips tooltips;

    for (auto assign : _assigns) assign->setToolTip(tooltips.assign_Assign());
    for (auto source : _sources) source->setToolTip(tooltips.assign_Source());
    for (auto sourceMode : _sourceModes) sourceMode->setToolTip(tooltips.assign_SourceMode());
    //for (auto target : _targets) target->setToolTip(tooltips.assign_Target());
    for (auto targetMin : _targetMins) targetMin->setToolTip(tooltips.assign_TargetMinMax());
    for (auto targetMax : _targetMaxs) targetMax->setToolTip(tooltips.assign_TargetMinMax());

#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::add_combo_items()
{
    // ----- ASSIGN -----
    for (auto source : _sources) {
        BossRc500::AssignSource(source);
    }

    for (auto sourceMode : _sourceModes) {
        BossRc500::AssignSourceMode(sourceMode);
    }

    for (auto target : _targets) {
        BossRc500::AssignTarget(target);
    }
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Apply, &QPushButton::clicked, this, [this] { apply = true; _parent.close(); });
    QObject::connect(button_Cancel, &QPushButton::clicked, [this] { apply = false; _parent.close(); });

    QObject::connect(button_targetHelp, &QPushButton::pressed, this, &BossRc500AssignDialog::on_target_help);

    // Control callbacks
    auto ConnectCheckBox_Assign = [&](const std::vector<QCheckBox*>& cbs, const char* name) {
        int index = 0;
        for (auto cb : cbs) {
            QObject::connect(cb, &QCheckBox::stateChanged,
                    this, [this, cb, index, name] {
                        on_Assign_CheckBox_changed(cb, index, name);
                    });
            ++index;
        }
    };

    // Assign1-8 Sw
    ConnectCheckBox_Assign(_assigns, "Sw");

    auto ConnectComboBox = [&](const std::vector<QComboBox*>& cbs, const char* name) {
        int index = 0;
        for (auto cb : cbs) {
            QObject::connect(cb, &QComboBox::currentIndexChanged,
                    this, [this, cb, index, name] { on_Assign_ComboBox_changed(cb, index, name);
                    });
            ++index;
        }
    };

    // Assign1-8
    ConnectComboBox(_sources, "Source");
    ConnectComboBox(_sourceModes, "SourceMode");

    int index = 0;
    for (auto target : _targets) {
        QObject::connect(target, &QComboBox::currentIndexChanged,
                this, [this, target, index] { on_Assign_Target_ComboBox_changed(target, index, "Target"); });
        ++index;
    }

    ConnectComboBox(_targetMins, "TargetMin");
    ConnectComboBox(_targetMaxs, "TargetMax");
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::load_assign()
{
    auto load = [this](int index) {
        auto& assign = database["mem"][_memory_index]["ASSIGN"][index];
        _assigns[index]->setChecked(assign["Sw"].get<int>());
        _sources[index]->setCurrentIndex(assign["Source"].get<int>());
        _sourceModes[index]->setCurrentIndex(assign["SourceMode"].get<int>());
        _targets[index]->setCurrentIndex(assign["Target"].get<int>());
        _targetMins[index]->setCurrentIndex(assign["TargetMin"].get<int>());
        _targetMaxs[index]->setCurrentIndex(assign["TargetMax"].get<int>());

        on_Assign_Target_ComboBox_changed(_targets[index], index, "Target");
    };

    for (int i = 0; i < 8; ++i) {
        load(i);
    }
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_target_help()
{
    try {
        QDialog dialog;
        Ui::DialogText textDialog;
        textDialog.setupUi(&dialog);

        BossRc500::Tooltips tooltips;
        textDialog.textEdit->setHtml(tooltips.assign_Target());

        dialog.setWindowTitle("BOSS RC-500 - Assign Target Help");
        dialog.setModal(true);
        dialog.exec();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_Target_ComboBox_changed(QComboBox* cb, int index, const char* name)
{
    on_Assign_ComboBox_changed(cb, index, name);

    auto target_index = cb->currentIndex();

    // Change available items for the current target
    auto targetMin = _targetMins[index];
    auto targetMax = _targetMaxs[index];

    targetMin->clear();
    targetMax->clear();

    switch(target_index) {
        case 4: // T1 REVERSE
        case 5: // T1 1SHOT
        case 11: // T1 LOOP.S
        case 12: // T1 TEMPO.S
        case 19: // T2 REVERSE
        case 20: // T2 1SHOT
        case 26: // T2 LOOP.S
        case 27: // T2 TEMPO.S
        case 35: // CUR REVERSE
        case 36: // CUR 1SHOT
        case 42: // CUR LOOP.S
        case 43: // CUR TEMPO.S
        case 54: // AUTO REC
        case 63: // LOOP FX
        case 64: // TR1 FX
        case 65: // TR2 FX
        case 66: // CUR TR FX
        case 85: // RHY FILL
        case 86: // RHY PART 1-4
        case 87: // RHY PART 1-4
        case 88: // RHY PART 1-4
        case 89: // RHY PART 1-4
        {
            BossRc500::OffOnMinMax(targetMin, targetMax);
            break;
        }
        case 6: // T1 LEVEL1
        case 21: // T2 LEVEL1
        case 37: // CUR LEVEL1
        case 74: // RHYTHM LEV1
        case 94: // MEMORY LEV1
        {
            BossRc500::LevelMinMax(targetMin, targetMax);
            break;
        }
        case 8: // T1 PAN
        case 23: // T2 PAN
        case 39: // CUR PAN
        {
            BossRc500::PanMinMax(targetMin, targetMax);
            break;
        }
        case 9: // T1 START
        case 24: // T2 START
        case 40: // CUR START
        {
            BossRc500::StartMinMax(targetMin, targetMax);
            break;
        }
        case 10: // T1 STOP
        case 25: // T2 STOP
        case 41: // CUR STOP
        {
            BossRc500::StopMinMax(targetMin, targetMax);
            break;
        }
        case 13: // T1 INPUT
        case 28: // T2 INPUT
        case 44: // CUR INPUT
        {
            BossRc500::InputMinMax(targetMin, targetMax);
            break;
        }
        case 14: // T1 OUTPUT
        case 29: // T2 OUTPUT
        case 45: // CUR OUTPUT
        {
            BossRc500::OutputMinMax(targetMin, targetMax);
            break;
        }
        case 30: // TRK SELECT
        {
            BossRc500::TrkSelectMinMax(targetMin, targetMax);
            break;
        }
        case 51: // TEMPO
        {
            BossRc500::TempoMinMax(targetMin, targetMax);
            break;
        }
        case 52: // DUB MOD
        {
            BossRc500::DubModeMinMax(targetMin, targetMax);
            break;
        }
        case 53: // REC ACTION
        {
            BossRc500::RecActionMinMax(targetMin, targetMax);
            break;
        }
        case 55: // A. REC SENS
        {
            BossRc500::RecSensMinMax(targetMin, targetMax);
            break;
        }
        case 56: // REC SOURCE ALL
        {
            BossRc500::RecSourceMinMax(targetMin, targetMax);
            break;
        }
        case 57: // LOOP LENGTH
        {
            BossRc500::LoopLengthMinMax(targetMin, targetMax);
            break;
        }
        case 58: // PLAY MODE
        {
            BossRc500::PlayModeMinMax(targetMin, targetMax);
            break;
        }
        case 59: // SINGLE CHANGE
        {
            BossRc500::SingleChangeMinMax(targetMin, targetMax);
            break;
        }
        case 60: // FADE TIME
        {
            BossRc500::FadeTimeMinMax(targetMin, targetMax);
            break;
        }
        case 61: // ALL START TRK
        {
            BossRc500::AllStartMinMax(targetMin, targetMax);
            break;
        }
        case 62: // TRK CHAIN
        {
            BossRc500::TrkChainMinMax(targetMin, targetMax);
            break;
        }
        case 67: // FX TYPE
        {
            BossRc500::LoopFxTypeMinMax(targetMin, targetMax);
            break;
        }
        case 70: // FX CONTROL
        {
            BossRc500::AssignFxControlMinMax(targetMin, targetMax, _beat, _loopfxType);
            break;
        }
        case 76: // REVERB
        {
            BossRc500::ReverbMinMax(targetMin, targetMax);
            break;
        }
        case 77: // RHY PATTERN
        {
            BossRc500::RhythmPatternMinMax(targetMin, targetMax, _beat);
            break;
        }
        case 78: // VARIATION
        {
            BossRc500::RhythmVariationMinMax(targetMin, targetMax);
            break;
        }
        case 79: // VAR CHANGE
        {
            BossRc500::RhythmVarChangeMinMax(targetMin, targetMax);
            break;
        }
        case 80: // KIT
        {
            BossRc500::RhythmKitMinMax(targetMin, targetMax);
            break;
        }
        case 81: // RHY START
        {
            BossRc500::RhythmStartMinMax(targetMin, targetMax);
            break;
        }
        case 82: // RHY STOP
        {
            BossRc500::RhythmStopMinMax(targetMin, targetMax);
            break;
        }
        case 83: // REC COUNT
        {
            BossRc500::RhythmRecCountMinMax(targetMin, targetMax);
            break;
        }
        case 84: // PLAY COUNT
        {
            BossRc500::RhythmPlayCountMinMax(targetMin, targetMax);
            break;
        }
        case 90: // TONE LOW
        case 91: // TONE HIGH
        {
            BossRc500::RhythmToneMinMax(targetMin, targetMax);
            break;
        }
        default: {
            if (target_index >= 96) {
                BossRc500::AssignMidiMinMax(targetMin, targetMax);
            }
            else {
                BossRc500::NotAvailableMinMax(targetMin, targetMax);
            }
            break;
        }
    }
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_CheckBox_changed(QCheckBox* cb, int index, const char* name)
{
    update_mem_database(index, name, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_ComboBox_changed(QComboBox* cb, int index, const char* name)
{
    update_mem_database(index, name, cb->currentIndex(), cb);
}
