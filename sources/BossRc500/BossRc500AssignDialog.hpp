#pragma once

#include "Designer/DialogAssign.hpp"

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossRc500AssignDialog : public Ui_BossRc500AssignDialog, public QObject
{
public:
    explicit BossRc500AssignDialog(QDialog& dialog,
                                   const nlohmann::json& database,
                                   int mem_index);

    bool apply = false;
    nlohmann::json  database;

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void load_assign();

    void on_Assign_CheckBox_changed(QCheckBox* cb, int index, const char* name);
    void on_Assign_ComboBox_changed(QComboBox* cb, int index, const char* name);
    void on_Assign_SpinBox_changed(QSpinBox* cb, int index, const char* name);

private:
    QDialog&        _parent;
    int             _memory_index{0};
};
