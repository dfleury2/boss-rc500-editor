#include "DialogBoss.hpp"
#include <BossReaderWriter/BossReaderWriter.hpp>

#include <yaml-cpp/yaml.h>
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

    // Add MEMORY
    for (int i = 1; i <= 100; ++i) {
        cb_Memory->addItem(QString(std::to_string(i).c_str()));
        cb_CopyFrom->addItem(QString(std::to_string(i).c_str()));
        cb_CopyTo->addItem(QString(std::to_string(i).c_str()));
    }

    // Clear Filename
    label_Filename->setText(QString());

    // Add callbacks
    QObject::connect(button_Open, &QPushButton::pressed, this, &BossCopierDialog::on_open);
    QObject::connect(button_Save, &QPushButton::pressed, this, &BossCopierDialog::on_save);
    QObject::connect(button_Quit, &QPushButton::pressed, this, &BossCopierDialog::on_quit);

    QObject::connect(cb_Memory, &QComboBox::currentIndexChanged, this, &BossCopierDialog::on_memory_changed);

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


    // PAN
    auto pan_labels = [](QComboBox* cb) {
        for (int i = 0; i <= 100; ++i) {
            std::string pan_label = "CENTER";
            if (i < 50) pan_label = "L" + std::to_string(50 - i);
            else if (i > 50) pan_label = "R" + std::to_string(i - 50);

            cb->addItem(pan_label.c_str());
        }
    };
    pan_labels(track1_Pan);
    pan_labels(track2_Pan);

    // MEASURE
    auto measure_labels = [](QComboBox* cb) {
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
    measure_labels(track1_Measure);
    measure_labels(track2_Measure);

    // Read combo list
    try {
        YAML::Node lookup = YAML::LoadFile("./resources/lookup.yaml");

        if (lookup["track"]) {
            if (auto start = lookup["track"]["start"]) {
                for (auto it = start.begin(); it != start.end(); ++it) {
                    track1_Start->addItem(it->as<std::string>().c_str());
                    track2_Start->addItem(it->as<std::string>().c_str());
                }
            }
            if (auto stop = lookup["track"]["stop"]) {
                for (auto it = stop.begin(); it != stop.end(); ++it) {
                    track1_Stop->addItem(it->as<std::string>().c_str());
                    track2_Stop->addItem(it->as<std::string>().c_str());
                }
            }
            if (auto input = lookup["track"]["input"]) {
                for (auto it = input.begin(); it != input.end(); ++it) {
                    track1_Input->addItem(it->as<std::string>().c_str());
                    track2_Input->addItem(it->as<std::string>().c_str());
                }
            }
            if (auto output = lookup["track"]["output"]) {
                for (auto it = output.begin(); it != output.end(); ++it) {
                    track1_Output->addItem(it->as<std::string>().c_str());
                    track2_Output->addItem(it->as<std::string>().c_str());
                }
            }
        }
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
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

        if (auto response = QMessageBox().question(nullptr,
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

    auto& track1 = _database["mem"][memory_index]["TRACK"][0];

    track1_Reverse->setChecked(track1["Rev"].get<int>());
    track1_Level->setValue(track1["PlyLvl"].get<int>());
    track1_Pan->setCurrentIndex(track1["Pan"].get<int>());
    track1_OneShot->setChecked(track1["One"].get<int>());
    track1_LoopFx->setChecked(track1["LoopFx"].get<int>());
    track1_Start->setCurrentIndex(track1["StrtMod"].get<int>());
    track1_Stop->setCurrentIndex(track1["StpMod"].get<int>());
    //track1_Measure->...
    track1_LoopSync->setChecked(track1["LoopSync"].get<int>());
    track1_TempoSync->setChecked(track1["TempoSync"].get<int>());
    track1_Input->setCurrentIndex(track1["Input"].get<int>());
    track1_Output->setCurrentIndex(track1["Output"].get<int>());

    auto& track2 = _database["mem"][memory_index]["TRACK"][1];

    track2_Reverse->setChecked(track1["Rev"].get<int>());
    track2_Level->setValue(track1["PlyLvl"].get<int>());
    track2_Pan->setCurrentIndex(track1["Pan"].get<int>());
    track2_OneShot->setChecked(track1["One"].get<int>());
    track2_LoopFx->setChecked(track1["LoopFx"].get<int>());
    track2_Start->setCurrentIndex(track1["StrtMod"].get<int>());
    track2_Stop->setCurrentIndex(track1["StpMod"].get<int>());
    //track2_Measure->...
    track2_LoopSync->setChecked(track1["LoopSync"].get<int>());
    track2_TempoSync->setChecked(track1["TempoSync"].get<int>());
    track2_Input->setCurrentIndex(track1["Input"].get<int>());
    track2_Output->setCurrentIndex(track1["Output"].get<int>());
}
