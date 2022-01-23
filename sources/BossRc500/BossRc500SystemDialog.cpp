#include "BossRc500SystemDialog.hpp"
#include "BossRc500.hpp"
#include "Designer/ui_Boss-rc500-text.h"

#include <QMessageBox>

// --------------------------------------------------------------------------
BossRc500SystemDialog::BossRc500SystemDialog(QDialog& dialog, const nlohmann::json& db,
        const QString& language) :
        _parent(dialog), database(db), _language(language)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    _parent.setFixedSize(_parent.width(), _parent.height());

    _font_bold = _parent.font();
    _font_bold.setWeight(QFont::Weight::Bold);

    _is_loading = true;

    try {
        add_tooltips();
        add_combo_items();
        add_callbacks();


        load_system();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }

    _is_loading = false;
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::add_tooltips()
{
// Add tooltips
#if QT_CONFIG(tooltip)
    BossRc500::Tooltips tooltips(_language);

    general_DisplayContrast->setToolTip(tooltips.general_DisplayContrast());
    general_DisplayMode->setToolTip(tooltips.general_DisplayMode());
    general_UndoRedo->setToolTip(tooltips.general_UndoRedo());

    pref_Pedal1Pref->setToolTip(tooltips.pref_PedalPref());
    pref_Pedal2Pref->setToolTip(tooltips.pref_PedalPref());
    pref_Pedal3Pref->setToolTip(tooltips.pref_PedalPref());
    pref_Ctl1Pref->setToolTip(tooltips.pref_CtlPref());
    pref_Ctl2Pref->setToolTip(tooltips.pref_CtlPref());
    pref_ExpPref->setToolTip(tooltips.pref_ExpPref());

    extent_Extent->setToolTip(tooltips.extent_Extent());
    extent_Extent1Min->setToolTip(tooltips.extent_ExtentMin());
    extent_Extent1Max->setToolTip(tooltips.extent_ExtentMax());
    extent_Extent2Min->setToolTip(tooltips.extent_ExtentMin());
    extent_Extent2Max->setToolTip(tooltips.extent_ExtentMax());
    extent_Extent3Min->setToolTip(tooltips.extent_ExtentMin());
    extent_Extent3Max->setToolTip(tooltips.extent_ExtentMax());
    extent_Extent4Min->setToolTip(tooltips.extent_ExtentMin());
    extent_Extent4Max->setToolTip(tooltips.extent_ExtentMax());
    extent_Extent5Min->setToolTip(tooltips.extent_ExtentMin());
    extent_Extent5Max->setToolTip(tooltips.extent_ExtentMax());

#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::add_combo_items()
{
    // General
    BossRc500::DisplayContrast(general_DisplayContrast);
    BossRc500::DisplayMode(general_DisplayMode);
    BossRc500::UndoRedo(general_UndoRedo);

    // Extent
    BossRc500::Extent(extent_Extent);
    BossRc500::ExtentMin(extent_Extent1Min);
    BossRc500::ExtentMax(extent_Extent1Max);
    BossRc500::ExtentMin(extent_Extent2Min);
    BossRc500::ExtentMax(extent_Extent2Max);
    BossRc500::ExtentMin(extent_Extent3Min);
    BossRc500::ExtentMax(extent_Extent3Max);
    BossRc500::ExtentMin(extent_Extent4Min);
    BossRc500::ExtentMax(extent_Extent4Max);
    BossRc500::ExtentMin(extent_Extent5Min);
    BossRc500::ExtentMax(extent_Extent5Max);

    // Preference
    BossRc500::PedalCtlExpPref(pref_Pedal1Pref);
    BossRc500::PedalCtlExpPref(pref_Pedal2Pref);
    BossRc500::PedalCtlExpPref(pref_Pedal3Pref);
    BossRc500::PedalCtlExpPref(pref_Ctl1Pref);
    BossRc500::PedalCtlExpPref(pref_Ctl2Pref);
    BossRc500::PedalCtlExpPref(pref_ExpPref);

    // Midi
    BossRc500::RxCtlChannel(midi_RxCtlChannel);
    BossRc500::RxNoteChannel(midi_RxNoteCh);
    BossRc500::TxChannel(midi_TxChannel);
    BossRc500::SyncClock(midi_SyncClock);
    BossRc500::SyncStart(midi_SyncStart);

    BossRc500::MidiThru(midi_MidiThru);
    BossRc500::MidiThru(midi_UsbThru);

    // Input
    BossRc500::MicroIn(input_MicroIn);
    BossRc500::InstrumentIn(input_InstrumentIn);
    BossRc500::RhythmOut(input_Rhythm);
    BossRc500::PeakSource(input_PeakSource);

    // Control
    BossRc500::ControlPdlCtl(control_Pedal1);
    BossRc500::ControlPdlCtl(control_Pedal2);
    BossRc500::ControlPdlCtl(control_Pedal3);
    BossRc500::ControlPdlCtl(control_Control1);
    BossRc500::ControlPdlCtl(control_Control2);

    BossRc500::ControlExpr(control_Expression);
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Apply, &QPushButton::clicked, this, [this] { apply = true; _parent.close(); });
    QObject::connect(button_Cancel, &QPushButton::clicked, [this] { apply = false; _parent.close(); });
    QObject::connect(button_pdlctlHelp, &QPushButton::pressed, this, &BossRc500SystemDialog::on_control_pdlctl_help);

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
    ConnectComboBox(general_UndoRedo,  "SETUP", "UndoRedo");
    ConnectComboBox(extent_Extent,     "SETUP", "ExtentCh");
    ConnectComboBox(extent_Extent1Min, "SETUP", "Extent1Min");
    ConnectComboBox(extent_Extent1Max, "SETUP", "Extent1Max");
    ConnectComboBox(extent_Extent2Min, "SETUP", "Extent2Min");
    ConnectComboBox(extent_Extent2Max, "SETUP", "Extent2Max");
    ConnectComboBox(extent_Extent3Min, "SETUP", "Extent3Min");
    ConnectComboBox(extent_Extent3Max, "SETUP", "Extent3Max");
    ConnectComboBox(extent_Extent4Min, "SETUP", "Extent4Min");
    ConnectComboBox(extent_Extent4Max, "SETUP", "Extent4Max");
    ConnectComboBox(extent_Extent5Min, "SETUP", "Extent5Min");
    ConnectComboBox(extent_Extent5Max,  "SETUP", "Extent5Max");

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
    ConnectComboBox(control_Expression, "CTL", "Exp");
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
BossRc500SystemDialog::on_control_pdlctl_help()
{
    try {
        QDialog dialog;
        Ui::DialogText textDialog;
        textDialog.setupUi(&dialog);

        BossRc500::Tooltips tooltips;
        textDialog.textEdit->setHtml(tooltips.control_PdlCtl());

        dialog.setWindowTitle("BOSS RC-500 - Assign Pdl Ctl Help");
        dialog.setModal(true);
        dialog.exec();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}


// --------------------------------------------------------------------------
void
BossRc500SystemDialog::on_System_CheckBox_changed(QCheckBox* cb, const char* node, const char* name)
{
    update_sys_database(node, name, cb->isChecked(), cb);
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::on_System_ComboBox_changed(QComboBox *cb, const char* node, const char* name)
{
    update_sys_database(node, name, cb->currentIndex(), cb);
}

// --------------------------------------------------------------------------
void
BossRc500SystemDialog::on_System_SpinBox_changed(QSpinBox *cb, const char* node, const char* name)
{
    update_sys_database(node, name, cb->value(), cb);
}
