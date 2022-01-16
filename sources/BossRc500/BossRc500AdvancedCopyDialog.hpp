#pragma once

#include "Designer/ui_Boss-rc500-copy.h"

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossRc500AdvancedCopyDialog : public Ui::BossRc500CopyDialog, public QObject
{
public:
    explicit BossRc500AdvancedCopyDialog(QDialog& dialog,
                                   nlohmann::json& database,
                                   int mem_index);

private:
    void setup();
    void add_combo_items();
    void add_callbacks();

    void on_copy();

private:
    QDialog&        _parent;
    int             _memory_index{0};
    nlohmann::json& _database;
};
