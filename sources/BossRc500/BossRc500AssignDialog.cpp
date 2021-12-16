#include "BossRc500AssignDialog.hpp"

#include <QStyleFactory>

#include <iostream>

namespace {
// --------------------------------------------------------------------------
void
AddItemsToComboBox(QComboBox *cb, std::initializer_list<const char *> list) {
    for (auto &item: list) {
        cb->addItem(item);
    }
}
}

// --------------------------------------------------------------------------
BossRc500AssignDialog::BossRc500AssignDialog(QDialog& dialog,
                                             const nlohmann::json& db,
                                             int mem_index) :
        _parent(dialog), database(db), _memory_index(mem_index)
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
    assign1_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    assign2_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    assign3_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    assign4_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    assign5_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    assign6_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    assign7_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));
    assign8_Assign->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_assign.png\"/></p></body></html>", nullptr));

    assign1_Source->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));
    assign2_Source->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));
    assign4_Source->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));
    assign5_Source->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));
    assign6_Source->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));
    assign7_Source->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));
    assign8_Source->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_source.png\"/></p></body></html>", nullptr));

    assign1_SourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));
    assign2_SourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));
    assign4_SourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));
    assign5_SourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));
    assign6_SourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));
    assign7_SourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));
    assign8_SourceMode->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_sourcemode.png\"/></p></body></html>", nullptr));

    assign1_Target->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));
    assign2_Target->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));
    assign4_Target->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));
    assign5_Target->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));
    assign6_Target->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));
    assign7_Target->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));
    assign8_Target->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_target.png\"/></p></body></html>", nullptr));

    assign1_TargetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign2_TargetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign4_TargetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign5_TargetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign6_TargetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign7_TargetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign8_TargetMin->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));

    assign1_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign2_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign4_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign5_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign6_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign7_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
    assign8_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::add_combo_items()
{
    // ----- ASSIGN -----
    auto source_items = {
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
    };
    AddItemsToComboBox(assign1_Source, source_items);
    AddItemsToComboBox(assign2_Source, source_items);
    AddItemsToComboBox(assign3_Source, source_items);
    AddItemsToComboBox(assign4_Source, source_items);
    AddItemsToComboBox(assign5_Source, source_items);
    AddItemsToComboBox(assign6_Source, source_items);
    AddItemsToComboBox(assign7_Source, source_items);
    AddItemsToComboBox(assign8_Source, source_items);

    auto sourceMode_items = {"MOMENT", "TOGGLE"};
    AddItemsToComboBox(assign1_SourceMode, sourceMode_items);
    AddItemsToComboBox(assign2_SourceMode, sourceMode_items);
    AddItemsToComboBox(assign3_SourceMode, sourceMode_items);
    AddItemsToComboBox(assign4_SourceMode, sourceMode_items);
    AddItemsToComboBox(assign5_SourceMode, sourceMode_items);
    AddItemsToComboBox(assign6_SourceMode, sourceMode_items);
    AddItemsToComboBox(assign7_SourceMode, sourceMode_items);
    AddItemsToComboBox(assign8_SourceMode, sourceMode_items);

    auto target_Items = {
            "T1 REC/PLY", "T1 PLY/STP", "T1 CLEAR", "T1 UND/RED", "T1 REVERSE", "T1 1SHOT", "T1 LEVEL1", "T1 LEVEL2",
            "T1 PAN", "T1 START", "T1 STOP", "T1 LOOP.S", "T1 TEMPO.S", "T1 TEMPO.S", "T1 INPUT", "T1 OUTPUT",
            "T2 REC/PLY", "T2 PLY/STP", "T2 CLEAR", "T2 UND/RED", "T2 REVERSE", "T2 1SHOT", "T2 LEVEL1", "T2 LEVEL2",
            "T2 PAN", "T2 START", "T2 STOP", "T2 LOOP.S", "T2 TEMPO.S", "T2 TEMPO.S", "T2 INPUT", "T2 OUTPUT",
            "TRK SELECT", "CUR REC/PLY", "CUR CLEAR", "CUR UND/RED", "CUR REVERSE", "CUR 1SHOT", "CUR LEVEL1", "CUR LEVEL2",
            "CUR PAN", "CUR START", "CUR STOP", "CUR LOOP.S", "CUR LOOP.S", "CUR TEMPO.S", "CUR INPUT", "CUR OUTPUT",
            "UNDO/REDO", "ALL START", "TAP TEMPO", "TEMPO UP", "TEMPO DOWN", "TEMPO", "DUB MOD", "REC ACTION",
            "QUANTIZE", "AUTO REC", "A. REC SENS", "A. REC SRC", "LOOP LENGTH", "PLAY MODE", "SINGL CHNGE", "FADE TIME",
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
            "CC 90", "CC 91", "CC 92", "CC 93", "CC 94", "CC 95"    };
    AddItemsToComboBox(assign1_Target, target_Items);
    AddItemsToComboBox(assign2_Target, target_Items);
    AddItemsToComboBox(assign3_Target, target_Items);
    AddItemsToComboBox(assign4_Target, target_Items);
    AddItemsToComboBox(assign5_Target, target_Items);
    AddItemsToComboBox(assign6_Target, target_Items);
    AddItemsToComboBox(assign7_Target, target_Items);
    AddItemsToComboBox(assign8_Target, target_Items);

}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Apply, &QPushButton::clicked, this, [this] { apply = true; _parent.close(); });
    QObject::connect(button_Cancel, &QPushButton::clicked, [this] { apply = false; _parent.close(); });

    // Assign1-8 Sw
    QObject::connect(assign1_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign1_Assign, 0, "Sw"); });
    QObject::connect(assign2_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign2_Assign, 1, "Sw"); });
    QObject::connect(assign3_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign3_Assign, 2, "Sw"); });
    QObject::connect(assign4_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign4_Assign, 3, "Sw"); });
    QObject::connect(assign5_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign5_Assign, 4, "Sw"); });
    QObject::connect(assign6_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign6_Assign, 5, "Sw"); });
    QObject::connect(assign7_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign7_Assign, 6, "Sw"); });
    QObject::connect(assign8_Assign, &QCheckBox::stateChanged, this, [this] { on_Assign_CheckBox_changed(assign8_Assign, 7, "Sw"); });

    // Assign1-8 Source
    QObject::connect(assign1_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign1_Source, 0, "Source"); });
    QObject::connect(assign2_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign2_Source, 1, "Source"); });
    QObject::connect(assign3_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign3_Source, 2, "Source"); });
    QObject::connect(assign4_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign4_Source, 3, "Source"); });
    QObject::connect(assign5_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign5_Source, 4, "Source"); });
    QObject::connect(assign6_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign6_Source, 5, "Source"); });
    QObject::connect(assign7_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign7_Source, 6, "Source"); });
    QObject::connect(assign8_Source, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign8_Source, 7, "Source"); });

    // Assign1-8 SourceMode
    QObject::connect(assign1_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign1_SourceMode, 0, "SourceMode"); });
    QObject::connect(assign2_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign2_SourceMode, 1, "SourceMode"); });
    QObject::connect(assign3_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign3_SourceMode, 2, "SourceMode"); });
    QObject::connect(assign4_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign4_SourceMode, 3, "SourceMode"); });
    QObject::connect(assign5_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign5_SourceMode, 4, "SourceMode"); });
    QObject::connect(assign6_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign6_SourceMode, 5, "SourceMode"); });
    QObject::connect(assign7_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign7_SourceMode, 6, "SourceMode"); });
    QObject::connect(assign8_SourceMode, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign8_SourceMode, 7, "SourceMode"); });

    // Assign1-8 Target
    QObject::connect(assign1_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign1_Target, 0, "Target"); });
    QObject::connect(assign2_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign2_Target, 1, "Target"); });
    QObject::connect(assign3_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign3_Target, 2, "Target"); });
    QObject::connect(assign4_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign4_Target, 3, "Target"); });
    QObject::connect(assign5_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign5_Target, 4, "Target"); });
    QObject::connect(assign6_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign6_Target, 5, "Target"); });
    QObject::connect(assign7_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign7_Target, 6, "Target"); });
    QObject::connect(assign8_Target, &QComboBox::currentIndexChanged, this, [this] { on_Assign_ComboBox_changed(assign8_Target, 7, "Target"); });

    // Assign1-8 TargetMin
    QObject::connect(assign1_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign1_TargetMin, 0, "TargetMin"); });
    QObject::connect(assign2_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign2_TargetMin, 1, "TargetMin"); });
    QObject::connect(assign3_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign3_TargetMin, 2, "TargetMin"); });
    QObject::connect(assign4_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign4_TargetMin, 3, "TargetMin"); });
    QObject::connect(assign5_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign5_TargetMin, 4, "TargetMin"); });
    QObject::connect(assign6_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign6_TargetMin, 5, "TargetMin"); });
    QObject::connect(assign7_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign7_TargetMin, 6, "TargetMin"); });
    QObject::connect(assign8_TargetMin, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign8_TargetMin, 7, "TargetMin"); });

    // Assign1-8 TargetMax
    QObject::connect(assign1_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign1_TargetMax, 0, "TargetMax"); });
    QObject::connect(assign2_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign2_TargetMax, 1, "TargetMax"); });
    QObject::connect(assign3_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign3_TargetMax, 2, "TargetMax"); });
    QObject::connect(assign4_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign4_TargetMax, 3, "TargetMax"); });
    QObject::connect(assign5_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign5_TargetMax, 4, "TargetMax"); });
    QObject::connect(assign6_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign6_TargetMax, 5, "TargetMax"); });
    QObject::connect(assign7_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign7_TargetMax, 6, "TargetMax"); });
    QObject::connect(assign8_TargetMax, &QSpinBox::valueChanged, this, [this] { on_Assign_SpinBox_changed(assign8_TargetMax, 7, "TargetMax"); });
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::load_assign()
{
    auto& assign1 = database["mem"][_memory_index]["ASSIGN"][0];
    assign1_Assign->setChecked(assign1["Sw"].get<int>());
    assign1_Source->setCurrentIndex(assign1["Source"].get<int>());
    assign1_SourceMode->setCurrentIndex(assign1["SourceMode"].get<int>());
    assign1_Target->setCurrentIndex(assign1["Target"].get<int>());
    assign1_TargetMin->setValue(assign1["TargetMin"].get<int>());
    assign1_TargetMax->setValue(assign1["TargetMax"].get<int>());

    auto& assign2 = database["mem"][_memory_index]["ASSIGN"][1];
    assign2_Assign->setChecked(assign2["Sw"].get<int>());
    assign2_Source->setCurrentIndex(assign2["Source"].get<int>());
    assign2_SourceMode->setCurrentIndex(assign2["SourceMode"].get<int>());
    assign2_Target->setCurrentIndex(assign2["Target"].get<int>());
    assign2_TargetMin->setValue(assign2["TargetMin"].get<int>());
    assign2_TargetMax->setValue(assign2["TargetMax"].get<int>());

    auto& assign3 = database["mem"][_memory_index]["ASSIGN"][2];
    assign3_Assign->setChecked(assign3["Sw"].get<int>());
    assign3_Source->setCurrentIndex(assign3["Source"].get<int>());
    assign3_SourceMode->setCurrentIndex(assign3["SourceMode"].get<int>());
    assign3_Target->setCurrentIndex(assign3["Target"].get<int>());
    assign3_TargetMin->setValue(assign3["TargetMin"].get<int>());
    assign3_TargetMax->setValue(assign3["TargetMax"].get<int>());

    auto& assign4 = database["mem"][_memory_index]["ASSIGN"][3];
    assign4_Assign->setChecked(assign4["Sw"].get<int>());
    assign4_Source->setCurrentIndex(assign4["Source"].get<int>());
    assign4_SourceMode->setCurrentIndex(assign4["SourceMode"].get<int>());
    assign4_Target->setCurrentIndex(assign4["Target"].get<int>());
    assign4_TargetMin->setValue(assign4["TargetMin"].get<int>());
    assign4_TargetMax->setValue(assign4["TargetMax"].get<int>());

    auto& assign5 = database["mem"][_memory_index]["ASSIGN"][4];
    assign5_Assign->setChecked(assign5["Sw"].get<int>());
    assign5_Source->setCurrentIndex(assign5["Source"].get<int>());
    assign5_SourceMode->setCurrentIndex(assign5["SourceMode"].get<int>());
    assign5_Target->setCurrentIndex(assign5["Target"].get<int>());
    assign5_TargetMin->setValue(assign5["TargetMin"].get<int>());
    assign5_TargetMax->setValue(assign5["TargetMax"].get<int>());

    auto& assign6 = database["mem"][_memory_index]["ASSIGN"][5];
    assign6_Assign->setChecked(assign6["Sw"].get<int>());
    assign6_Source->setCurrentIndex(assign6["Source"].get<int>());
    assign6_SourceMode->setCurrentIndex(assign6["SourceMode"].get<int>());
    assign6_Target->setCurrentIndex(assign6["Target"].get<int>());
    assign6_TargetMin->setValue(assign6["TargetMin"].get<int>());
    assign6_TargetMax->setValue(assign6["TargetMax"].get<int>());

    auto& assign7 = database["mem"][_memory_index]["ASSIGN"][6];
    assign7_Assign->setChecked(assign7["Sw"].get<int>());
    assign7_Source->setCurrentIndex(assign7["Source"].get<int>());
    assign7_SourceMode->setCurrentIndex(assign7["SourceMode"].get<int>());
    assign7_Target->setCurrentIndex(assign7["Target"].get<int>());
    assign7_TargetMin->setValue(assign7["TargetMin"].get<int>());
    assign7_TargetMax->setValue(assign7["TargetMax"].get<int>());

    auto& assign8 = database["mem"][_memory_index]["ASSIGN"][7];
    assign8_Assign->setChecked(assign8["Sw"].get<int>());
    assign8_Source->setCurrentIndex(assign8["Source"].get<int>());
    assign8_SourceMode->setCurrentIndex(assign8["SourceMode"].get<int>());
    assign8_Target->setCurrentIndex(assign8["Target"].get<int>());
    assign8_TargetMin->setValue(assign8["TargetMin"].get<int>());
    assign8_TargetMax->setValue(assign8["TargetMax"].get<int>());
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_CheckBox_changed(QCheckBox *cb, int index, const char *name)
{
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (_memory_index + 1) << ", ASSIGN: " << index << ": " << name << ": " << value << std::endl;
    database["mem"][_memory_index]["ASSIGN"][index][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_ComboBox_changed(QComboBox *cb, int index, const char *name)
{
    int value =cb->currentIndex();
    std::cout << "Memory: " << (_memory_index + 1) << ", ASSIGN: " << index << ": " << name << ": " << value << std::endl;
    database["mem"][_memory_index]["ASSIGN"][index][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500AssignDialog::on_Assign_SpinBox_changed(QSpinBox *cb, int index, const char *name)
{
    int value =cb->value();
    std::cout << "Memory: " << (_memory_index + 1) << ", ASSIGN: " << index << ": " << name << ": " << value << std::endl;
    database["mem"][_memory_index]["ASSIGN"][index][name] = value;
}
