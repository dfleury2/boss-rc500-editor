#pragma once

#include "Designer/DialogSystem.hpp"

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossRc500SystemDialog : public Ui_BossRc500SystemDialog, public QObject
{
public:
    explicit BossRc500SystemDialog(QDialog& dialog, const nlohmann::json& database);

    bool apply = false;
    nlohmann::json  database;

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void load_system();

    void on_System_CheckBox_changed(QCheckBox* cb, const char* node, const char* name);
    void on_System_ComboBox_changed(QComboBox* cb, const char* node, const char* name);
    void on_System_SpinBox_changed(QSpinBox* cb, const char* node, const char* name);

private:
    QDialog&        _parent;
};
