#include "DialogBoss.hpp"
#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>

#include <iostream>
#include <initializer_list>

namespace {
void AddItemsToComboBox(QComboBox* cb, std::initializer_list<const char*> list) {
    for (auto& item : list) {
        cb->addItem(item);
    }
}

}

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
    QApplication::setStyle(QStyleFactory::create("Fusion"));
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
#endif // QT_CONFIG(tooltip)
}

// --------------------------------------------------------------------------
void
BossCopierDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Open, &QPushButton::pressed, this, &BossCopierDialog::on_open);
    QObject::connect(button_Copy, &QPushButton::pressed, this, &BossCopierDialog::on_copy);
    QObject::connect(button_Save, &QPushButton::pressed, this, &BossCopierDialog::on_save);
    QObject::connect(button_Quit, &QPushButton::pressed, this, &BossCopierDialog::on_quit);

    QObject::connect(cb_Memory, &QComboBox::currentIndexChanged, this, &BossCopierDialog::on_memory_changed);

    // Track callbacks
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

}

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

    // ----- TRACK 1/2 -----
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
        AddItemsToComboBox(cb, {"IMMEDIATE", "FADE IN"});
    };
    start_items(track1_Start);
    start_items(track2_Start);

    // STOP
    auto stop_items = [](QComboBox* cb) {
        AddItemsToComboBox(cb, {"IMMEDIATE", "FADE OUT", "LOOP END"});
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
        AddItemsToComboBox(cb, {"ALL", "MIC IN", "INST IN", "INST IN-A", "INST IN-B", "MIC/INST"});
    };
    input_items(track1_Input);
    input_items(track2_Input);

    // OUTPUT
    auto output_items = [](QComboBox* cb) {
        AddItemsToComboBox(cb, {"ALL", "OUT-A", "OUT-B"});
    };
    output_items(track1_Output);
    output_items(track2_Output);

    // ----- RECORD -----
    AddItemsToComboBox(record_DubMode, {"OVERDUB", "REPLACE"});
    AddItemsToComboBox(record_RecordAction, {"REC -> DUB", "REC -> PLAY"});
    AddItemsToComboBox(record_Quantize, {"OFF", "MEASURE"});
    AddItemsToComboBox(record_AutoRecordSource, {"ALL", "MIC IN", "INST", "INST-A", "INST-B"});

    record_LoopLength->addItem("AUTO");
    for (int i = 1; i <= 32; ++i)
        record_LoopLength->addItem(std::to_string(i).c_str());

    // ----- PLAY -----
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
BossCopierDialog::on_copy()
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
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
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

        if (auto response = QMessageBox::question(nullptr,
                "Write changes to file ?",
                QString::fromStdString("Do you want to write to file " + filename));
                response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        WriteMemoryDatabase(_database, filename);

        QMessageBox(QMessageBox::Information, "Information", "Database successfully written to file.").exec();
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
BossCopierDialog::on_Level_changed(QSlider* slider)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (slider == track1_Level ? 0 : 1);
    int value = slider->value();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Level: " << value << std::endl;

    auto value_str = std::to_string(value);
    _database["mem"][memory_index]["TRACK"][track_index]["PlyLvl"] = value;

    slider->setToolTip(value_str.c_str());
    (slider == track1_Level ? label_track1_PlyLevel : label_track2_PlyLevel)->setText(value_str.c_str());
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_Pan_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Pan ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Pan: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Pan"] = value;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_Start_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Start ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Start: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["StrtMod"] = value;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_Stop_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Stop ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Stop: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["StpMod"] = value;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_Measure_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Measure ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Measure: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Measure"] = value;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_Reverse_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Reverse ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", Rev: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Rev"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_LoopFx_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_LoopFx ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", LoopFx: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["LoopFx"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_OneShot_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_OneShot ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", OneShot: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["One"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_LoopSync_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_LoopSync ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", LoopSync: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["LoopSync"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_TempoSync_changed(QCheckBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_TempoSync ? 0 : 1);
    int is_checked = (cb->isChecked() ? 1 : 0);
    std::cout << "Memory: " << (memory_index + 1)
            << ", Track: " << (track_index + 1 )
            << ", TempoSync: " << is_checked << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["TempoSync"] = is_checked;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_Input_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Input ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Input: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Input"] = value;
}

// --------------------------------------------------------------------------
void
BossCopierDialog::on_Output_changed(QComboBox* cb)
{
    int memory_index = cb_Memory->currentIndex();
    int track_index = (cb == track1_Output ? 0 : 1);
    int value = cb->currentIndex();
    std::cout << "Memory: " << (memory_index + 1)
              << ", Track: " << (track_index + 1 )
              << ", Output: " << value << std::endl;

    _database["mem"][memory_index]["TRACK"][track_index]["Output"] = value;
}
