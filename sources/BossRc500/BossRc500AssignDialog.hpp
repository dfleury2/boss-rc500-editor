#pragma once

#include "Designer/DialogAssign.hpp"
#include "BossRc500.hpp"

#include <nlohmann/json.hpp>
#include <vector>

// --------------------------------------------------------------------------
class BossRc500AssignDialog : public Ui_BossRc500AssignDialog, public QObject
{
public:
    explicit BossRc500AssignDialog(QDialog& dialog,
                                   nlohmann::json database,
                                   int mem_index,
                                   const BossRc500::Beat& beat);

    bool apply = false;
    nlohmann::json  database;

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void load_assign();

    void on_Assign_Target_ComboBox_changed(QComboBox* cb, int index, const char* name);

    void on_Assign_CheckBox_changed(QCheckBox* cb, int index, const char* name);
    void on_Assign_ComboBox_changed(QComboBox* cb, int index, const char* name);

private:
    QDialog&        _parent;
    int             _memory_index{0};
    BossRc500::Beat _beat;

    std::vector<QCheckBox*> _assigns;
    std::vector<QComboBox*> _sources;
    std::vector<QComboBox*> _sourceModes;
    std::vector<QComboBox*> _targets;
    std::vector<QComboBox*> _targetMins;
    std::vector<QComboBox*> _targetMaxs;
};
