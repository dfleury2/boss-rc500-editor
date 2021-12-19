#include "BossRc500SystemDialog.hpp"

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
BossRc500SystemDialog::BossRc500SystemDialog(QDialog& dialog, const nlohmann::json& db) :
        _parent(dialog), database(db)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    _parent.setFixedSize(_parent.width(), _parent.height());

    add_tooltips();
    add_combo_items();
    add_callbacks();

    load_system();
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::add_tooltips()
{
// Add tooltips
#if QT_CONFIG(tooltip)
//    assign1_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
//    assign2_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
//    assign4_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
//    assign5_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
//    assign6_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
//    assign7_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
//    assign8_TargetMax->setToolTip(QCoreApplication::translate("BossRc500Dialog", "<html><head/><body><p><img src=\"./resources/tooltips/assign_targetminmax.png\"/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::add_combo_items()
{
    AddItemsToComboBox(general_DisplayContrast, {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"});
    AddItemsToComboBox(general_DisplayMode,
            {"STATUS", "POSITION", "2TRACK POS", "STATUS+POS",
             "NUMBER+POS", "NAME+POS", "BEAT+POS", "BEAT"});
    AddItemsToComboBox(general_UndoRedo, {"HOLD", "RELEASE"});

    for (int i = 1; i < 100; ++i) {
        extent_Extent1Min->addItem(std::to_string(i).c_str());
        extent_Extent1Max->addItem(std::to_string(i).c_str());
        extent_Extent2Min->addItem(std::to_string(i).c_str());
        extent_Extent2Max->addItem(std::to_string(i).c_str());
        extent_Extent3Min->addItem(std::to_string(i).c_str());
        extent_Extent3Max->addItem(std::to_string(i).c_str());
        extent_Extent4Min->addItem(std::to_string(i).c_str());
        extent_Extent4Max->addItem(std::to_string(i).c_str());
        extent_Extent5Min->addItem(std::to_string(i).c_str());
        extent_Extent5Max->addItem(std::to_string(i).c_str());
    }

    auto pref_Pedal = {"MEMORY", "SYSTEM"};
    AddItemsToComboBox(pref_Pedal1Pref, pref_Pedal);
    AddItemsToComboBox(pref_Pedal2Pref, pref_Pedal);
    AddItemsToComboBox(pref_Pedal3Pref, pref_Pedal);
    AddItemsToComboBox(pref_Ctl1Pref, pref_Pedal);
    AddItemsToComboBox(pref_Ctl2Pref, pref_Pedal);
    AddItemsToComboBox(pref_ExpPref, pref_Pedal);

    AddItemsToComboBox(midi_RxCtlChannel,
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
             "11", "12", "13", "14", "15", "16"});
    AddItemsToComboBox(midi_RxNoteCh,
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
             "11", "12", "13", "14", "15", "16"});
    AddItemsToComboBox(midi_TxChannel,
            {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
             "11", "12", "13", "14", "15", "16", "RX CTL"});
    AddItemsToComboBox(midi_SyncClock, {"AUTO", "INTERNAL", "USB", "MIDI"});
    AddItemsToComboBox(midi_SyncStart, {"OFF", "ALL", "RHYTHM"});

    auto midiUsbThru_items = {"OFF", "MIDI OUT", "USB OUT", "USB/MIDI"};
    AddItemsToComboBox(midi_MidiThru, midiUsbThru_items);
    AddItemsToComboBox(midi_UsbThru, midiUsbThru_items);

    AddItemsToComboBox(extent_Extent, {"EXT1", "EXT2", "EXT3", "EXT4", "EXT5"});

    AddItemsToComboBox(input_MicroIn, {"OUT-A&B", "OUT-A", "OFF"});
    AddItemsToComboBox(input_InstrumentIn, {"OUT-A&B", "OUT-A", "OUT-B", "OFF"});
    AddItemsToComboBox(input_Rhythm, {"OUT-A&B", "OUT-A", "OUT-B", "LOOP", "OFF"});
    AddItemsToComboBox(input_PeakSource, {"ALL IN", "MIC IN", "INST IN", "LOOP&RHYTHM"});

    // ----- CTL -----
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
BossRc500SystemDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Apply, &QPushButton::clicked, this, [this] { apply = true; _parent.close(); });
    QObject::connect(button_Cancel, &QPushButton::clicked, [this] { apply = false; _parent.close(); });

    auto ConnectComboBox = [this](QComboBox* cb, const char* node, const char* name) {
        QObject::connect(cb, &QComboBox::currentIndexChanged,
                this, [this, cb, node, name] { on_System_ComboBox_changed(cb, node, name); });
    };
    auto ConnectCheckBox = [this](QCheckBox* cb, const char* node, const char* name) {
        QObject::connect(cb, &QCheckBox::stateChanged,
                this, [this, cb, node, name] { on_System_CheckBox_changed(cb, node, name); });
    };
    auto ConnectSpinBox = [this](QSpinBox* cb, const char* node, const char* name) {
        QObject::connect(cb, &QSpinBox::valueChanged,
                this, [this, cb, node, name] { on_System_SpinBox_changed(cb, node, name); });
    };

    // SETUP (General + Extent)
    ConnectComboBox(general_DisplayContrast, "SETUP", "Contrast");
    ConnectComboBox(general_DisplayMode, "SETUP", "DisplayMode");
    ConnectComboBox(general_UndoRedo, "SETUP", "UndoRedo");
    ConnectComboBox(extent_Extent, "SETUP", "ExtentCh");
    ConnectComboBox(extent_Extent1Min, "SETUP", "Extent1Min");
    ConnectComboBox(extent_Extent1Max, "SETUP", "Extent1Max");
    ConnectComboBox(extent_Extent2Min, "SETUP", "Extent2Min");
    ConnectComboBox(extent_Extent2Max, "SETUP", "Extent2Max");
    ConnectComboBox(extent_Extent3Min, "SETUP", "Extent3Min");
    ConnectComboBox(extent_Extent3Max, "SETUP", "Extent3Max");
    ConnectComboBox(extent_Extent4Min, "SETUP", "Extent4Min");
    ConnectComboBox(extent_Extent4Max, "SETUP", "Extent4Max");
    ConnectComboBox(extent_Extent5Min, "SETUP", "Extent5Min");
    ConnectComboBox(extent_Extent5Max, "SETUP", "Extent5Max");

    // PREF
    ConnectComboBox(pref_Pedal1Pref,    "PREF", "Pdl1");
    ConnectComboBox(pref_Pedal2Pref,    "PREF", "Pdl2");
    ConnectComboBox(pref_Pedal3Pref,    "PREF", "Pdl3");
    ConnectComboBox(pref_Ctl1Pref,      "PREF", "Ctl1");
    ConnectComboBox(pref_Ctl2Pref,      "PREF", "Ctl2");
    ConnectComboBox(pref_ExpPref,       "PREF", "Exp");

    // MIDI
    ConnectComboBox(midi_RxCtlChannel,  "MIDI", "RxCtlCh");
    ConnectCheckBox(midi_Omni,          "MIDI", "Omni");
    ConnectComboBox(midi_RxNoteCh,      "MIDI", "RxNoteCh");
    ConnectComboBox(midi_TxChannel,     "MIDI", "TxCh");
    ConnectComboBox(midi_SyncClock,     "MIDI", "Sync");
    ConnectCheckBox(midi_ClockOut,      "MIDI", "ClkOut");
    ConnectComboBox(midi_SyncStart,     "MIDI", "SyncStart");
    ConnectCheckBox(midi_PcOut,         "MIDI", "PcOut");
    ConnectComboBox(midi_MidiThru,      "MIDI", "MidiThru");
    ConnectComboBox(midi_UsbThru,       "MIDI", "UsbThru");

    // INPUT
    ConnectCheckBox(input_Phantom,      "INPUT", "Phantom");
    ConnectComboBox(input_MicroIn,      "INPUT", "MicOut");
    ConnectComboBox(input_InstrumentIn, "INPUT", "InstOut");
    ConnectComboBox(input_Rhythm,       "INPUT", "RhythmOut");
    ConnectSpinBox(input_MicroInNs,     "INPUT", "MicNs");
    ConnectSpinBox(input_InstrInNs,     "INPUT", "InstNs");
    ConnectComboBox(input_PeakSource,   "INPUT", "Indicator");

    // CTL
    ConnectComboBox(control_Pedal1,     "CTL", "Pedal1");
    ConnectComboBox(control_Pedal2,     "CTL", "Pedal2");
    ConnectComboBox(control_Pedal3,     "CTL", "Pedal3");
    ConnectComboBox(control_Control1,   "CTL", "Ctl1");
    ConnectComboBox(control_Control2,   "CTL", "Ctl2");
    ConnectComboBox(control_Expression,   "CTL", "Exp");
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::load_system()
{
    {
        auto& setup = database["sys"]["SETUP"];
        general_DisplayContrast->setCurrentIndex(setup["Contrast"].get<int>());
        general_DisplayMode->setCurrentIndex(setup["DisplayMode"].get<int>());
        general_UndoRedo->setCurrentIndex(setup["UndoRedo"].get<int>());

        extent_Extent->setCurrentIndex(setup["ExtentCh"].get<int>());

        extent_Extent1Min->setCurrentIndex(setup["Extent1Min"].get<int>());
        extent_Extent1Max->setCurrentIndex(setup["Extent1Max"].get<int>());
        extent_Extent2Min->setCurrentIndex(setup["Extent2Min"].get<int>());
        extent_Extent2Max->setCurrentIndex(setup["Extent2Max"].get<int>());
        extent_Extent3Min->setCurrentIndex(setup["Extent3Min"].get<int>());
        extent_Extent3Max->setCurrentIndex(setup["Extent3Max"].get<int>());
        extent_Extent4Min->setCurrentIndex(setup["Extent4Min"].get<int>());
        extent_Extent4Max->setCurrentIndex(setup["Extent4Max"].get<int>());
        extent_Extent5Min->setCurrentIndex(setup["Extent5Min"].get<int>());
        extent_Extent5Max->setCurrentIndex(setup["Extent5Max"].get<int>());
    }

    {
        auto& pref = database["sys"]["PREF"];
        pref_Pedal1Pref->setCurrentIndex(pref["Pdl1"].get<int>());
        pref_Pedal2Pref->setCurrentIndex(pref["Pdl2"].get<int>());
        pref_Pedal3Pref->setCurrentIndex(pref["Pdl3"].get<int>());
        pref_Ctl1Pref->setCurrentIndex(pref["Ctl1"].get<int>());
        pref_Ctl2Pref->setCurrentIndex(pref["Ctl2"].get<int>());
        pref_ExpPref->setCurrentIndex(pref["Exp"].get<int>());
    }

    {
        auto& midi = database["sys"]["MIDI"];
        midi_RxCtlChannel->setCurrentIndex(midi["RxCtlCh"].get<int>());
        midi_Omni->setChecked(midi["Omni"].get<int>());
        midi_RxNoteCh->setCurrentIndex(midi["RxNoteCh"].get<int>());
        midi_TxChannel->setCurrentIndex(midi["TxCh"].get<int>());
        midi_SyncClock->setCurrentIndex(midi["Sync"].get<int>());
        midi_ClockOut->setChecked(midi["ClkOut"].get<int>());
        midi_SyncStart->setCurrentIndex(midi["SyncStart"].get<int>());
        midi_PcOut->setChecked(midi["PcOut"].get<int>());
        midi_MidiThru->setCurrentIndex(midi["MidiThru"].get<int>());
        midi_UsbThru->setCurrentIndex(midi["UsbThru"].get<int>());
    }

    {
        auto& input = database["sys"]["INPUT"];
        input_Phantom->setChecked(input["Phantom"].get<int>());
        input_MicroIn->setCurrentIndex(input["MicOut"].get<int>());
        input_InstrumentIn->setCurrentIndex(input["InstOut"].get<int>());
        input_Rhythm->setCurrentIndex(input["RhythmOut"].get<int>());
        input_MicroInNs->setValue(input["MicNs"].get<int>());
        input_InstrInNs->setValue(input["InstNs"].get<int>());
        input_PeakSource->setCurrentIndex(input["Indicator"].get<int>());
    }

    {
        auto& ctl = database["sys"]["CTL"];
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
BossRc500SystemDialog::on_System_CheckBox_changed(QCheckBox* cb, const char* node, const char* name)
{
    int value = (cb->isChecked() ? 1 : 0);
    std::cout << "node: " << node << ", name: " << name << ": " << value << std::endl;
    database["sys"][node][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::on_System_ComboBox_changed(QComboBox *cb, const char* node, const char* name)
{
    int value = cb->currentIndex();
    std::cout << "node: " << node << ", name: " << name << ": " << value << std::endl;
    database["sys"][node][name] = value;
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::on_System_SpinBox_changed(QSpinBox *cb, const char* node, const char* name)
{
    int value = cb->value();
    std::cout << "node: " << node << ", name: " << name << ": " << value << std::endl;
    database["sys"][node][name] = value;
}
