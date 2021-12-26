#include "BossRc500AssignDialog.hpp"
#include "BossRc500.hpp"

#include <QStyleFactory>

#include <iostream>

// --------------------------------------------------------------------------
BossRc500AssignDialog::BossRc500AssignDialog(QDialog& dialog,
        nlohmann::json db,
        int mem_index,
        const BossRc500::Beat& beat) :
        _parent(dialog), database(std::move(db)), _memory_index(mem_index),
        _beat(beat)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    _parent.setFixedSize(_parent.width(), _parent.height());

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

    add_tooltips();
    add_combo_items();
    add_callbacks();

    load_assign();
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::add_tooltips()
{
// Add tooltips
#if QT_CONFIG(tooltip)
    for (auto assign : _assigns) {
        assign->setToolTip(QCoreApplication::translate("BossRc500Dialog",
                "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    }

    for (auto source : _sources) {
        source->setToolTip(QCoreApplication::translate("BossRc500Dialog",
                "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));
    }

    for (auto sourceMode : _sourceModes) {
        sourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog",
                "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));
    }

    for (auto target : _targets) {
        target->setToolTip(QCoreApplication::translate("BossRc500Dialog",
                "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));
    }

    for (auto targetMin : _targetMins) {
        targetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog",
                "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    }

    for (auto targetMax : _targetMaxs) {
        targetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog",
                "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    }
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
BossRc500AssignDialog::on_Assign_Target_ComboBox_changed(QComboBox* cb, int index, const char* name)
{
    on_Assign_ComboBox_changed(cb, index, name);

    auto target_index = cb->currentIndex();

    // Change available items for the current target
    auto targetMin = _targetMins[index];
    auto targetMax = _targetMaxs[index];
    auto current_index_min = targetMin->currentIndex();
    auto current_index_max = targetMax->currentIndex();

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
            BossRc500::OffOn(targetMin);
            BossRc500::OffOn(targetMax);
            break;
        }
        case 6: // T1 LEVEL1
        case 21: // T2 LEVEL1
        case 37: // CUR LEVEL1
        {
            BossRc500::Level(targetMin);
            BossRc500::Level(targetMax);
            break;
        }
        case 8: // T1 PAN
        case 23: // T2 PAN
        case 39: // CUR PAN
        {
            BossRc500::Pan(targetMin);
            BossRc500::Pan(targetMax);
            break;
        }
        case 9: // T1 START
        case 24: // T2 START
        case 40: // CUR START
        {
            BossRc500::Start(targetMin);
            BossRc500::Start(targetMax);
            break;
        }
        case 10: // T1 STOP
        case 25: // T2 STOP
        case 41: // CUR STOP
        {
            BossRc500::Stop(targetMin);
            BossRc500::Stop(targetMax);
            break;
        }
        case 13: // T1 INPUT
        case 28: // T2 INPUT
        case 44: // CUR INPUT
        {
            BossRc500::Input(targetMin);
            BossRc500::Input(targetMax);
            break;
        }
        case 14: // T1 OUTPUT
        case 29: // T2 OUTPUT
        case 45: // CUR OUTPUT
        {
            BossRc500::Output(targetMin);
            BossRc500::Output(targetMax);
            break;
        }
        case 30: // TRK SELECT
        {
            BossRc500::TrkSelect(targetMin);
            BossRc500::TrkSelect(targetMax);
            break;
        }
        case 51: // TEMPO
        {
            BossRc500::Tempo(targetMin);
            BossRc500::Tempo(targetMax);
            break;
        }
        case 52: // DUB MOD
        {
            BossRc500::DubMode(targetMin);
            BossRc500::DubMode(targetMax);
            break;
        }
        case 53: // REC ACTION
        {
            BossRc500::RecAction(targetMin);
            BossRc500::RecAction(targetMax);
            break;
        }
        case 55: // A. REC SENS
        {
            BossRc500::RecSens(targetMin);
            BossRc500::RecSens(targetMax);
            break;
        }
        case 56: // REC SOURCE ALL
        {
            BossRc500::RecSource(targetMin);
            BossRc500::RecSource(targetMax);
            break;
        }
        case 57: // LOOP LENGTH
        {
            BossRc500::LoopLength(targetMin);
            BossRc500::LoopLength(targetMax);
            break;
        }
        case 58: // PLAY MODE
        {
            BossRc500::PlayMode(targetMin);
            BossRc500::PlayMode(targetMax);
            break;
        }
        case 59: // SINGLE CHANGE
        {
            BossRc500::SingleChange(targetMin);
            BossRc500::SingleChange(targetMax);
            break;
        }
        case 60: // FADE TIME
        {
            BossRc500::FadeTime(targetMin);
            BossRc500::FadeTime(targetMax);
            break;
        }
        case 61: // ALL START TRK
        {
            BossRc500::AllStart(targetMin);
            BossRc500::AllStart(targetMax);
            break;
        }
        case 62: // TRK CHAIN
        {
            BossRc500::TrkChain(targetMin);
            BossRc500::TrkChain(targetMax);
            break;
        }
        case 67: // FX TYPE
        {
            BossRc500::LoopFxType(targetMin);
            BossRc500::LoopFxType(targetMax);
            break;
        }
        case 70: // FX CONTROL
        {
            BossRc500::AssignFxControl(targetMin, _beat);
            BossRc500::AssignFxControl(targetMax, _beat);
            break;
        }
        case 74: // RHYTHM LEV1
        {
            BossRc500::Level(targetMin);
            BossRc500::Level(targetMax);
            break;
        }
        case 76: // REVERB
        {
            BossRc500::Reverb(targetMin);
            BossRc500::Reverb(targetMax);
            break;
        }
        case 77: // RHY PATTERN
        {
            BossRc500::RhythmPattern(targetMin);
            BossRc500::RhythmPattern(targetMax);
            break;
        }
        case 78: // VARIATION
        {
            BossRc500::RhythmVariation(targetMin);
            BossRc500::RhythmVariation(targetMax);
            break;
        }
        case 79: // VAR CHANGE
        {
            BossRc500::RhythmVarChange(targetMin);
            BossRc500::RhythmVarChange(targetMax);
            break;
        }
        case 80: // KIT
        {
            BossRc500::RhythmKit(targetMin);
            BossRc500::RhythmKit(targetMax);
            break;
        }
        case 81: // RHY START
        {
            BossRc500::RhythmStart(targetMin);
            BossRc500::RhythmStart(targetMax);
            break;
        }
        case 82: // RHY STOP
        {
            BossRc500::RhythmStop(targetMin);
            BossRc500::RhythmStop(targetMax);
            break;
        }
        case 83: // REC COUNT
        {
            BossRc500::RhythmRecCount(targetMin);
            BossRc500::RhythmRecCount(targetMax);
            break;
        }
        case 84: // PLAY COUNT
        {
            BossRc500::RhythmPlayCount(targetMin);
            BossRc500::RhythmPlayCount(targetMax);
            break;
        }
        case 90: // TONE LOW
        {
            BossRc500::RhythmTone(targetMin);
            BossRc500::RhythmTone(targetMax);
            break;
        }
        case 91: // TONE HIGH
        {
            BossRc500::RhythmTone(targetMin);
            BossRc500::RhythmTone(targetMax);
            break;
        }
        case 94: // MEMORY LEV1
        {
            BossRc500::Level(targetMin);
            BossRc500::Level(targetMax);
            break;
        }
        default: {
            if (target_index >= 96) {
                BossRc500::IntegerRange(targetMin, 0, 127);
                BossRc500::IntegerRange(targetMax, 0, 127);
            }
            else {
                BossRc500::NotAvailable(targetMin);
                BossRc500::NotAvailable(targetMax);
            }
            break;
        }
    }
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_CheckBox_changed(QCheckBox* cb, int index, const char* name)
{
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (_memory_index + 1) << ", ASSIGN: " << index << ": " << name << ": " << value << std::endl;
    database["mem"][_memory_index]["ASSIGN"][index][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_ComboBox_changed(QComboBox* cb, int index, const char* name)
{
    int value = cb->currentIndex();
    std::cout << "Memory: " << (_memory_index + 1) << ", ASSIGN: " << index << ": " << name << ": " << value << std::endl;
    database["mem"][_memory_index]["ASSIGN"][index][name] = value;
}
