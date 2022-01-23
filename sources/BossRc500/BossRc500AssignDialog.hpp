#pragma once

#include "Designer/ui_Boss-rc500-assign.h"
#include "BossRc500.hpp"

#include <nlohmann/json.hpp>

#include <vector>
#include <iostream>

// --------------------------------------------------------------------------
class BossRc500AssignDialog : public Ui_BossRc500AssignDialog, public QObject
{
public:
    explicit BossRc500AssignDialog(QDialog& dialog,
                                   nlohmann::json database,
                                   int mem_index,
                                   const BossRc500::Beat& beat,
                                   int loopfxType,
                                   const QString& language);

    bool apply = false;
    nlohmann::json  database;

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void load_assign();

    void on_target_help();

    void on_Assign_Target_ComboBox_changed(QComboBox* cb, int index, const char* name);

    void on_Assign_CheckBox_changed(QCheckBox* cb, int index, const char* name);
    void on_Assign_ComboBox_changed(QComboBox* cb, int index, const char* name);

    template<typename Widget, typename Value>
    void update_mem_database(int index, const char* name, Value value, Widget* w)
    {
        if (!_is_loading) {
            std::cout << "Assign: " << name << ": " << value << std::endl;
            database["mem"][_memory_index]["ASSIGN"][index][name] = static_cast<int>(value);
        }

        auto default_value = BossRc500::DatabaseMemDefault["mem"][_memory_index]["ASSIGN"][index][name].get<int>();

        bool is_modified = false;

        if constexpr(std::is_floating_point_v<Value>) {
            is_modified = (std::abs(value - default_value) > 0.05);
        }
        else if constexpr(std::is_same_v<Value, bool>) {
            is_modified = (int(value) != default_value);
        }
        else {
            is_modified = (default_value != value);
        }

        w->setFont(is_modified ? _font_bold : _parent.font());
    }

private:
    QDialog&        _parent;
    int             _memory_index{0};
    BossRc500::Beat _beat;
    int             _loopfxType{0};
    QString         _language;

    std::vector<QCheckBox*> _assigns;
    std::vector<QComboBox*> _sources;
    std::vector<QComboBox*> _sourceModes;
    std::vector<QComboBox*> _targets;
    std::vector<QComboBox*> _targetMins;
    std::vector<QComboBox*> _targetMaxs;

    QFont           _font_bold; // different from default
    bool _is_loading = false; // Allow detecting on new/load on callback calls
};
