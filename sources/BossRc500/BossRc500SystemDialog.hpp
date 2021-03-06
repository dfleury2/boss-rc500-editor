#pragma once

#include "Designer/ui_Boss-rc500-system.h"
#include "BossRc500.hpp"

#include <nlohmann/json.hpp>

#include <iostream>

// --------------------------------------------------------------------------
class BossRc500SystemDialog : public Ui_BossRc500SystemDialog, public QObject
{
public:
    explicit BossRc500SystemDialog(QDialog& dialog, const nlohmann::json& database,
            const QString& language);

    bool apply = false;
    nlohmann::json  database;

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void load_system();

    void on_control_pdlctl_help();
    void on_System_CheckBox_changed(QCheckBox* cb, const char* node, const char* name);
    void on_System_ComboBox_changed(QComboBox* cb, const char* node, const char* name);
    void on_System_SpinBox_changed(QSpinBox* cb, const char* node, const char* name);

    template<typename Widget, typename Value>
    void update_sys_database(const char* root, const char* name, Value value, Widget* w)
    {
        if (!_is_loading) {
            std::cout << "System: " << root << "." << name << ": " << value << std::endl;
            database["sys"][root][name] = static_cast<int>(value);
        }

        auto default_value = BossRc500::DatabaseSysDefault["sys"][root][name].get<int>();

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
    QString         _language;
    QFont           _font_bold; // different from default
    bool _is_loading = false; // Allow detecting on new/load on callback calls
};
