#include "BossRc500SystemDialog.hpp"
#include "BossRc500.hpp"
#include "Designer/ui_Boss-rc500-text.h"

#include <QMessageBox>

// --------------------------------------------------------------------------
BossRc500SystemDialog::BossRc500SystemDialog(QDialog& dialog, const BossRC::SystemFile& db, const QString& language)
    : Ui_BossRc500SystemDialog(), database(db), _parent(dialog), _language(language)
{
    setup();
}

// --------------------------------------------------------------------------
void BossRc500SystemDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    _parent.setFixedSize(_parent.width(), _parent.height());

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
void BossRc500SystemDialog::add_tooltips() const
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

#endif   // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void BossRc500SystemDialog::add_combo_items()
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
void BossRc500SystemDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Apply, &QPushButton::clicked, this, [this] {
        apply = true;
        _parent.close();
    });
    QObject::connect(button_Cancel, &QPushButton::clicked, [this] {
        apply = false;
        _parent.close();
    });
    QObject::connect(button_pdlctlHelp, &QPushButton::pressed, this, &BossRc500SystemDialog::on_control_pdlctl_help);

    auto ConnectComboBox = [this](QComboBox* cb, auto node, auto member) {
        QObject::connect(cb, &QComboBox::currentIndexChanged, this,
                         [this, cb, node, member] { on_System_ComboBox_changed(cb, node, member); });
    };
    auto ConnectCheckBox = [this](QCheckBox* cb, auto node, auto name) {
        QObject::connect(cb, &QCheckBox::checkStateChanged, this, [this, cb, node, name] { on_System_CheckBox_changed(cb, node, name); });
    };
    auto ConnectSpinBox = [this](QSpinBox* cb, auto node, auto name) {
        QObject::connect(cb, &QSpinBox::valueChanged, this, [this, cb, node, name] { on_System_SpinBox_changed(cb, node, name); });
    };

    // SETUP (General + Extent)
    ConnectComboBox(general_DisplayContrast, &BossRC::SystemFile::setup, &BossRC::Setup::Contrast);
    ConnectComboBox(general_DisplayMode, &BossRC::SystemFile::setup, &BossRC::Setup::DisplayMode);
    ConnectComboBox(general_UndoRedo, &BossRC::SystemFile::setup, &BossRC::Setup::UndoRedo);
    ConnectComboBox(extent_Extent, &BossRC::SystemFile::setup, &BossRC::Setup::ExtentCh);
    ConnectComboBox(extent_Extent1Min, &BossRC::SystemFile::setup, &BossRC::Setup::Extent1Min);
    ConnectComboBox(extent_Extent1Max, &BossRC::SystemFile::setup, &BossRC::Setup::Extent1Max);
    ConnectComboBox(extent_Extent2Min, &BossRC::SystemFile::setup, &BossRC::Setup::Extent2Min);
    ConnectComboBox(extent_Extent2Max, &BossRC::SystemFile::setup, &BossRC::Setup::Extent2Max);
    ConnectComboBox(extent_Extent3Min, &BossRC::SystemFile::setup, &BossRC::Setup::Extent3Min);
    ConnectComboBox(extent_Extent3Max, &BossRC::SystemFile::setup, &BossRC::Setup::Extent3Max);
    ConnectComboBox(extent_Extent4Min, &BossRC::SystemFile::setup, &BossRC::Setup::Extent4Min);
    ConnectComboBox(extent_Extent4Max, &BossRC::SystemFile::setup, &BossRC::Setup::Extent4Max);
    ConnectComboBox(extent_Extent5Min, &BossRC::SystemFile::setup, &BossRC::Setup::Extent5Min);
    ConnectComboBox(extent_Extent5Max, &BossRC::SystemFile::setup, &BossRC::Setup::Extent5Max);

    // PREF
    ConnectComboBox(pref_Pedal1Pref, &BossRC::SystemFile::pref, &BossRC::Pref::Pdl1);
    ConnectComboBox(pref_Pedal2Pref, &BossRC::SystemFile::pref, &BossRC::Pref::Pdl2);
    ConnectComboBox(pref_Pedal3Pref, &BossRC::SystemFile::pref, &BossRC::Pref::Pdl3);
    ConnectComboBox(pref_Ctl1Pref, &BossRC::SystemFile::pref, &BossRC::Pref::Ctl1);
    ConnectComboBox(pref_Ctl2Pref, &BossRC::SystemFile::pref, &BossRC::Pref::Ctl2);
    ConnectComboBox(pref_ExpPref, &BossRC::SystemFile::pref, &BossRC::Pref::Exp);

    // MIDI
    ConnectComboBox(midi_RxCtlChannel, &BossRC::SystemFile::midi, &BossRC::Midi::RxCtlCh);
    ConnectCheckBox(midi_Omni, &BossRC::SystemFile::midi, &BossRC::Midi::Omni);
    ConnectComboBox(midi_RxNoteCh, &BossRC::SystemFile::midi, &BossRC::Midi::RxNoteCh);
    ConnectComboBox(midi_TxChannel, &BossRC::SystemFile::midi, &BossRC::Midi::TxCh);
    ConnectComboBox(midi_SyncClock, &BossRC::SystemFile::midi, &BossRC::Midi::Sync);
    ConnectCheckBox(midi_ClockOut, &BossRC::SystemFile::midi, &BossRC::Midi::ClkOut);
    ConnectComboBox(midi_SyncStart, &BossRC::SystemFile::midi, &BossRC::Midi::SyncStart);
    ConnectCheckBox(midi_PcOut, &BossRC::SystemFile::midi, &BossRC::Midi::PcOut);
    ConnectComboBox(midi_MidiThru, &BossRC::SystemFile::midi, &BossRC::Midi::MidiThru);
    ConnectComboBox(midi_UsbThru, &BossRC::SystemFile::midi, &BossRC::Midi::UsbThru);

    // INPUT
    ConnectCheckBox(input_Phantom, &BossRC::SystemFile::input, &BossRC::Input::Phantom);
    ConnectComboBox(input_MicroIn, &BossRC::SystemFile::input, &BossRC::Input::MicOut);
    ConnectComboBox(input_InstrumentIn, &BossRC::SystemFile::input, &BossRC::Input::InstOut);
    ConnectComboBox(input_Rhythm, &BossRC::SystemFile::input, &BossRC::Input::RhythmOut);
    ConnectSpinBox(input_MicroInNs, &BossRC::SystemFile::input, &BossRC::Input::MicNs);
    ConnectSpinBox(input_InstrInNs, &BossRC::SystemFile::input, &BossRC::Input::InstNs);
    ConnectComboBox(input_PeakSource, &BossRC::SystemFile::input, &BossRC::Input::Indicator);

    // CTL
    ConnectComboBox(control_Pedal1, &BossRC::SystemFile::ctl, &BossRC::Ctl::Pedal1);
    ConnectComboBox(control_Pedal2, &BossRC::SystemFile::ctl, &BossRC::Ctl::Pedal2);
    ConnectComboBox(control_Pedal3, &BossRC::SystemFile::ctl, &BossRC::Ctl::Pedal3);
    ConnectComboBox(control_Control1, &BossRC::SystemFile::ctl, &BossRC::Ctl::Ctl1);
    ConnectComboBox(control_Control2, &BossRC::SystemFile::ctl, &BossRC::Ctl::Ctl2);
    ConnectComboBox(control_Expression, &BossRC::SystemFile::ctl, &BossRC::Ctl::Exp);
}

// --------------------------------------------------------------------------
void BossRc500SystemDialog::load_system()
{
    {
        const auto& setup = database.setup;
        general_DisplayContrast->setCurrentIndex(setup.Contrast);
        general_DisplayMode->setCurrentIndex(setup.DisplayMode);
        general_UndoRedo->setCurrentIndex(setup.UndoRedo);

        extent_Extent->setCurrentIndex(setup.ExtentCh);

        extent_Extent1Min->setCurrentIndex(setup.Extent1Min);
        extent_Extent1Max->setCurrentIndex(setup.Extent1Max);
        extent_Extent2Min->setCurrentIndex(setup.Extent2Min);
        extent_Extent2Max->setCurrentIndex(setup.Extent2Max);
        extent_Extent3Min->setCurrentIndex(setup.Extent3Min);
        extent_Extent3Max->setCurrentIndex(setup.Extent3Max);
        extent_Extent4Min->setCurrentIndex(setup.Extent4Min);
        extent_Extent4Max->setCurrentIndex(setup.Extent4Max);
        extent_Extent5Min->setCurrentIndex(setup.Extent5Min);
        extent_Extent5Max->setCurrentIndex(setup.Extent5Max);
    }

    {
        const auto& pref = database.pref;
        pref_Pedal1Pref->setCurrentIndex(pref.Pdl1);
        pref_Pedal2Pref->setCurrentIndex(pref.Pdl2);
        pref_Pedal3Pref->setCurrentIndex(pref.Pdl3);
        pref_Ctl1Pref->setCurrentIndex(pref.Ctl1);
        pref_Ctl2Pref->setCurrentIndex(pref.Ctl2);
        pref_ExpPref->setCurrentIndex(pref.Exp);
    }

    {
        const auto& midi = database.midi;
        midi_RxCtlChannel->setCurrentIndex(midi.RxCtlCh);
        midi_Omni->setChecked(midi.Omni);
        midi_RxNoteCh->setCurrentIndex(midi.RxNoteCh);
        midi_TxChannel->setCurrentIndex(midi.TxCh);
        midi_SyncClock->setCurrentIndex(midi.Sync);
        midi_ClockOut->setChecked(midi.ClkOut);
        midi_SyncStart->setCurrentIndex(midi.SyncStart);
        midi_PcOut->setChecked(midi.PcOut);
        midi_MidiThru->setCurrentIndex(midi.MidiThru);
        midi_UsbThru->setCurrentIndex(midi.UsbThru);
    }

    {
        const auto& input = database.input;
        input_Phantom->setChecked(input.Phantom);
        input_MicroIn->setCurrentIndex(input.MicOut);
        input_InstrumentIn->setCurrentIndex(input.InstOut);
        input_Rhythm->setCurrentIndex(input.RhythmOut);
        input_MicroInNs->setValue(input.MicNs);
        input_InstrInNs->setValue(input.InstNs);
        input_PeakSource->setCurrentIndex(input.Indicator);
    }

    {
        const auto& ctl = database.ctl;
        control_Pedal1->setCurrentIndex(ctl.Pedal1);
        control_Pedal2->setCurrentIndex(ctl.Pedal2);
        control_Pedal3->setCurrentIndex(ctl.Pedal3);
        control_Control1->setCurrentIndex(ctl.Ctl1);
        control_Control2->setCurrentIndex(ctl.Ctl2);
        control_Expression->setCurrentIndex(ctl.Exp);
    }

    const std::initializer_list<QWidget*> widgets = {
        label_extent_Extent,
        extent_Extent,

        label_extent_Extent2,
        extent_Extent2Min,
        extent_Extent2Max,

        label_extent_Extent3,
        extent_Extent3Min,
        extent_Extent3Max,

        label_extent_Extent4,
        extent_Extent4Min,
        extent_Extent4Max,

        label_extent_Extent5,
        extent_Extent5Min,
        extent_Extent5Max,

        label_control_Pedal2,
        control_Pedal2,
        label_control_Pedal3,
        control_Pedal3,

        label_control_Expression,
        control_Expression,

        gp_Input,
        gp_Preference,
    };

    const auto Apply = [mode_rc500 = database.isBossRc500Mode()](const std::initializer_list<QWidget*> widgets) {
        for (const auto w : widgets) {
            if (mode_rc500) {
                w->show();
                w->setDisabled(false);
            }
            else {
                w->hide();
                w->setDisabled(true);
            }
        }
    };

    Apply(widgets);
}

// --------------------------------------------------------------------------
void BossRc500SystemDialog::on_control_pdlctl_help()
{
    try {
        QDialog dialog;
        Ui::DialogText textDialog;
        textDialog.setupUi(&dialog);

        BossRc500::Tooltips tooltips;
        textDialog.textEdit->setHtml(tooltips.control_PdlCtl());

        dialog.setWindowTitle("BOSS RC LOOP STATION - Assign Pdl Ctl Help");
        dialog.setModal(true);
        dialog.exec();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}
