#include "BossRc500AdvancedCopyDialog.hpp"

#include <QMessageBox>

#include <iostream>

// --------------------------------------------------------------------------
BossRc500AdvancedCopyDialog::BossRc500AdvancedCopyDialog(QDialog& dialog,
        nlohmann::json& db,
        int mem_index) :
        _parent(dialog), _database(db), _memory_index(mem_index)
{
    setup();
}

// --------------------------------------------------------------------------
void
BossRc500AdvancedCopyDialog::setup()
{
    setupUi(&_parent);

    // Add some tweaks...
    _parent.setFixedSize(_parent.width(), _parent.height());

    try {
        add_callbacks();
        add_combo_items();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}

// --------------------------------------------------------------------------
void
BossRc500AdvancedCopyDialog::add_combo_items()
{
    // MEMORY
    for (int i = 1; i <= 99; ++i) {
        cb_CopyFrom->addItem(std::to_string(i).c_str());
        cb_CopyTo->addItem(std::to_string(i).c_str());
    }
}

// --------------------------------------------------------------------------
void
BossRc500AdvancedCopyDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Copy, &QPushButton::pressed, this, &BossRc500AdvancedCopyDialog::on_copy);
    QObject::connect(button_Close, &QPushButton::clicked, this, [this] { _parent.close(); });
}

// --------------------------------------------------------------------------
void
BossRc500AdvancedCopyDialog::on_copy()
{
    try {
        int copy_from_slot = cb_CopyFrom->currentIndex();
        int copy_to_slot = cb_CopyTo->currentIndex();

        if (copy_from_slot > copy_to_slot) {
            throw std::runtime_error("Copy From slot must be less than copy to slot");
        }

        if (auto response = QMessageBox::question(nullptr,
                    "Copy memories ?",
                    QString::fromStdString("Do you want to copy memory slot " + std::to_string(_memory_index + 1) +
                            "\ninto slots from " + std::to_string(copy_from_slot + 1) + " to " +
                            std::to_string(copy_to_slot + 1))); response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        // Update database and write it to disk
        const auto& slot = _database["mem"][_memory_index];

        for (int i = copy_from_slot; i <= copy_to_slot; ++i) {
            auto& mem = _database["mem"][i];

            // Save values to be restored after the copy
            auto backup = mem;

            // Copy some values
            if (copy_Track1->isChecked()) {
                mem["TRACK"][0] = slot["TRACK"][0];
            }
            if (copy_Track2->isChecked()) {
                mem["TRACK"][1] = slot["TRACK"][1];
            }
            if (copy_Rhythm->isChecked()) {
                mem["RHYTHM"] = slot["RHYTHM"];
            }
            if (copy_LoopFx->isChecked()) {
                mem["LOOPFX"] = slot["LOOPFX"];
            }
            if (copy_Control->isChecked()) {
                mem["CTL"] = slot["CTL"];
            }

            // Master is split between Record and Play
            auto master_copy = [&](std::initializer_list<const char*> names) {
                for (auto&& name : names) {
                    mem["MASTER"][name] = slot["MASTER"][name];
                }
            };
            if (copy_Record->isChecked()) {
                master_copy({"DubMode", "RecAction", "RecQuantize", "AutoRec",
                             "AutoRecSens", "AutoRecSrc", "LpLen"}); // LpMod ???
            }
            if (copy_Play->isChecked()) {
                master_copy({"Tempo", "PlayMode", "SinglPlayeChange", "Level",
                             "FadeTime", "AllStart", "TrackChain"});
            }
            // Sync, TrkMode, CurrentTrack, AllTrackSel are missing somewhere...

            // Restore some values from the original
            mem["id"] = backup["id"];
            mem["NAME"] = backup["NAME"];
            mem["name"] = backup["name"];
        }

        _parent.close();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "", ex.what()).exec();
    }
}
