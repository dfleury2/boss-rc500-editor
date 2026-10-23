#pragma once

#include "Designer/ui_Boss-rc500-copy.h"

#include <BossReaderWriter/BossRC.hpp>

// --------------------------------------------------------------------------
class BossRc500AdvancedCopyDialog : public Ui::BossRc500CopyDialog, public QObject
{
public:
    explicit BossRc500AdvancedCopyDialog(QDialog& dialog,
                                   BossRC::MemoryFile& database,
                                   int mem_index);

private:
    void setup();
    void add_combo_items() const;
    void add_callbacks();

    void on_copy();

private:
    QDialog&        _parent;
    int             _memory_index{0};
    BossRC::MemoryFile& _database;
};
