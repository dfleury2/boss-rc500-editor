#include "BossRc500AdvancedCopyDialog.hpp"

#include <QMessageBox>

#include <fmt/format.h>

// --------------------------------------------------------------------------
BossRc500AdvancedCopyDialog::BossRc500AdvancedCopyDialog(QDialog& dialog, BossRC::MemoryFile& database, const int mem_index)
    : _parent(dialog), _memory_index(mem_index), _database(database)
{
    setup();
}

// --------------------------------------------------------------------------
void BossRc500AdvancedCopyDialog::setup()
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
void BossRc500AdvancedCopyDialog::add_combo_items() const
{
    // MEMORY
    for (int i = 1; i <= 99; ++i) {
        cb_CopyFrom->addItem(std::to_string(i).c_str());
        cb_CopyTo->addItem(std::to_string(i).c_str());
    }
}

// --------------------------------------------------------------------------
void BossRc500AdvancedCopyDialog::add_callbacks()
{
    // Add callbacks
    QObject::connect(button_Copy, &QPushButton::pressed, this, &BossRc500AdvancedCopyDialog::on_copy);
    QObject::connect(button_Close, &QPushButton::clicked, this, [this] { _parent.close(); });
}

// --------------------------------------------------------------------------
void BossRc500AdvancedCopyDialog::on_copy()
{
    try {
        const int copy_from_slot = cb_CopyFrom->currentIndex();
        const int copy_to_slot = cb_CopyTo->currentIndex();

        if (copy_from_slot > copy_to_slot) {
            throw std::runtime_error("Copy From slot must be less than copy to slot");
        }

        if (const auto response = QMessageBox::question(nullptr, "Copy memories ?",
                                                        fmt::format("Do you want to copy memory slot {}\ninto slots from {} to {}",
                                                                    _memory_index + 1, copy_from_slot + 1, copy_to_slot + 1)
                                                            .c_str());
            response != QMessageBox::Yes) {
            throw std::runtime_error("Operation canceled");
        }

        // Update database
        const auto& slot = _database.memories[_memory_index];

        for (int i = copy_from_slot; i <= copy_to_slot; ++i) {
            auto& mem = _database.memories[i];

            // Save values to be restored after the copy
            const auto backup = mem;

            // Copy some values
            if (copy_Track1->isChecked()) {
                mem.tracks[0] = slot.tracks[0];
            }
            if (copy_Track2->isChecked()) {
                mem.tracks[1] = slot.tracks[1];
            }
            if (copy_Rhythm->isChecked()) {
                mem.rhythm = slot.rhythm;
            }
            if (copy_LoopFx->isChecked()) {
                mem.loopFx = slot.loopFx;
            }
            if (copy_Control->isChecked()) {
                mem.ctl = slot.ctl;
            }
            if (copy_Assign->isChecked()) {
                mem.assigns = slot.assigns;
            }

            // Master is split between Record and Play
            auto master_copy = [&](auto name) { mem.master.*name = slot.master.*name; };
            if (copy_Record->isChecked()) {
                master_copy(&BossRC::Master::DubMode);
                master_copy(&BossRC::Master::RecAction);
                master_copy(&BossRC::Master::RecQuantize);
                master_copy(&BossRC::Master::AutoRec);
                master_copy(&BossRC::Master::AutoRecSens);
                master_copy(&BossRC::Master::AutoRecSrc);
                master_copy(&BossRC::Master::LpLen);
                // LpMod ???
            }
            if (copy_Play->isChecked()) {
                master_copy(&BossRC::Master::Tempo);
                master_copy(&BossRC::Master::PlayMode);
                master_copy(&BossRC::Master::SinglPlayeChange);
                master_copy(&BossRC::Master::Level);
                master_copy(&BossRC::Master::FadeTime);
                master_copy(&BossRC::Master::AllStart);
                master_copy(&BossRC::Master::TrackChain);
            }
            // Sync, TrkMode, CurrentTrack, AllTrackSel are missing somewhere...

            // Restore some values from the original
            mem.name = backup.name;
        }

        _parent.close();
    }
    catch (const std::exception& ex) {
        QMessageBox(QMessageBox::Warning, "Error on copy", ex.what()).exec();
    }
}
