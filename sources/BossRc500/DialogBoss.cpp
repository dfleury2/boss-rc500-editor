#include "DialogBoss.hpp"
#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QFileDialog>
#include <QMessageBox>

#include <iostream>

// --------------------------------------------------------------------------
BossCopierDialog::BossCopierDialog(QDialog& dialog) :
        _parent(dialog)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossCopierDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    _parent.setFixedSize(_parent.width(), _parent.height());
    label_Filename->setText(QString());

    add_tooltips();
    add_callbacks();
    add_combo_items();
}

// --------------------------------------------------------------------------
void
BossCopierDialog::add_tooltips()
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
#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossCopierDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Open, &QPushButton::pressed, this, &BossCopierDialog::on_open);
    QObject::connect(button_Save, &QPushButton::pressed, this, &BossCopierDialog::on_save);
    QObject::connect(button_Quit, &QPushButton::pressed, this, &BossCopierDialog::on_quit);

    QObject::connect(cb_Memory, &QComboBox::currentIndexChanged, this, &BossCopierDialog::on_memory_changed);

    // Track callbacks
    QObject::connect(track1_Reverse, &QCheckBox::stateChanged, this, [this] { on_reverse_changed(track1_Reverse); });
    QObject::connect(track2_Reverse, &QCheckBox::stateChanged, this, [this] { on_reverse_changed(track2_Reverse); });
}

// --------------------------------------------------------------------------
// Some combobox items can be computed and nor hard coded
// --------------------------------------------------------------------------
void
BossCopierDialog::add_combo_items()
{
    // MEMORY
    for (int i = 1; i <= 100; ++i) {
        cb_Memory->addItem(QString(std::to_string(i).c_str()));
        cb_CopyFrom->addItem(QString(std::to_string(i).c_str()));
        cb_CopyTo->addItem(QString(std::to_string(i).c_str()));
    }

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
        cb->addItem("IMMEDIATE");
        cb->addItem("FADE IN");
    };
    start_items(track1_Start);
    start_items(track2_Start);

    // STOP
    auto stop_items = [](QComboBox* cb) {
        cb->addItem("IMMEDIATE");
        cb->addItem("FADE OUT");
        cb->addItem("LOOP END");
    };
    stop_items(track1_Stop);
    stop_items(track2_Stop);

    // MEASURE
    auto measure_items = [](QComboBox* cb) {
        cb->addItem("AUTO");
        cb->addItem("FREE");
        cb->addItem(QIcon("./resources/images/semi-quaver.png"), "Semi-quaver");
        cb->addItem(QIcon("./resources/images/quaver.png"), "Quaver");
        cb->addItem(QIcon("./resources/images/quarter note.png"), "Quarter Note");
        cb->addItem(QIcon("./resources/images/half note.png"), "Half Note");
        for (int i = 1; i <= 16; ++i) {
            cb->addItem(std::to_string(i).c_str());
        }
    };
    measure_items(track1_Measure);
    measure_items(track2_Measure);

    // INPUT
    auto input_items = [](QComboBox* cb) {
        cb->addItem("ALL");
        cb->addItem("MIC IN");
        cb->addItem("INST IN");
        cb->addItem("INST IN-A");
        cb->addItem("INST IN-B");
        cb->addItem("MIC/INST");
    };
    input_items(track1_Input);
    input_items(track2_Input);

    // OUTPUT
    auto output_items = [](QComboBox* cb) {
        cb->addItem("ALL");
        cb->addItem("OUT-A");
        cb->addItem("OUT-B");
    };
    output_items(track1_Output);
    output_items(track2_Output);
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_open()
{
    try {
        auto filename = QFileDialog::getOpenFileName(&_parent,
                tr("Open a MEMORY file"), "", tr("Memory Files (*.RC0)"));

        label_Filename->setText(filename);
        if (!filename.isEmpty()) {
            _database = ReadMemoryDatabase(filename.toStdString());

            cb_Memory->setCurrentIndex(0);
            load_memory();
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
        label_Filename->clear();
    }
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_save()
{
    try {
        auto filename = label_Filename->text().toStdString();
        if (filename.empty()) {
            throw std::runtime_error("No filename selected");
        }

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
        const auto& slot = _database["mem"][memory_slot - 1];

        for (int i = copy_from_slot; i <= copy_to_slot; ++i) {
            _database["mem"][i - 1] = slot;
            // Restore the memory id of the copied slot
            _database["mem"][i - 1]["id"] = i - 1;
        }

        WriteMemoryDatabase(_database, filename);
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_quit()
{
    QApplication::exit();
}

// --------------------------------------------------------------------------
void BossCopierDialog::on_memory_changed()
{
    auto filename = label_Filename->text().toStdString();
    if (!filename.empty()) {
        load_memory();
    }
}

// --------------------------------------------------------------------------
void
BossCopierDialog::load_memory()
{
    int memory_index = cb_Memory->currentIndex();

    { // TRACK 1
        auto& track1 = _database["mem"][memory_index]["TRACK"][0];

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
        auto& track2 = _database["mem"][memory_index]["TRACK"][1];

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
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_reverse_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Reverse ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", Rev: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Rev"]  = is_checked;
}
