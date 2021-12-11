#include "DialogBoss.hpp"
#include "BossReaderWriter.hpp"

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

    QObject::connect(button_Open, &QPushButton::pressed, this, &BossCopierDialog::on_open);
    QObject::connect(button_Save, &QPushButton::pressed, this, &BossCopierDialog::on_save);
    QObject::connect(button_Quit, &QPushButton::pressed, this, &BossCopierDialog::on_quit);

    // Read combo list
    try {
        YAML::Node lookup = YAML::LoadFile("./resources/lookup.yaml");

        if (lookup["track"]) {
            if (auto pan = lookup["track"]["pan"]) {
                for (auto it = pan.begin(); it != pan.end(); ++it) {
                    track1_Pan->addItem(it->as<std::string>().c_str());
                    track2_Pan->addItem(it->as<std::string>().c_str());
                }
            }
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
