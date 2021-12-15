#include "DialogBoss.hpp"
#include <BossReaderWriter/BossReaderWriter.hpp>

#include <QFileDialog>
#include <QMessageBox>

// --------------------------------------------------------------------------
BossRc500MainDialog::BossRc500MainDialog(QDialog& dialog) :
        _parent(dialog)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::setup()
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

    QObject::connect(button_Open, &QPushButton::pressed, this, &BossRc500MainDialog::on_ToolMenu_Open);
    QObject::connect(button_Save, &QPushButton::pressed, this, &BossRc500MainDialog::on_ToolMenu_Save);
    QObject::connect(button_Quit, &QPushButton::pressed, this, &BossRc500MainDialog::on_ToolMenu_Quit);
}

// --------------------------------------------------------------------------
void
BossRc500MainDialog::on_ToolMenu_Open()
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
BossRc500MainDialog::on_ToolMenu_Save()
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
BossRc500MainDialog::on_ToolMenu_Quit()
{
    QApplication::exit();
}
