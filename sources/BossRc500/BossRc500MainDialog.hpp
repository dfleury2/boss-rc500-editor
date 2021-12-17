#pragma once

#include "Designer/Dialog.hpp"

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossRc500MainDialog : public Ui_BossRc500MainDialog, public QObject
{
public:
    explicit BossRc500MainDialog(QDialog& dialog);

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void on_ToolMenu_New();
    void on_ToolMenu_Open();
    void on_ToolMenu_Save(bool askFilename = false);
    void on_ToolMenu_Assign();

    void on_edit();
    void on_copy();

    void on_memory_changed();
    void load_database(const std::string& filename);
    void load_memory(int memory_index);

    // Track 1/2 callbacks
    void on_Level_changed(QSlider* slider);
    void on_Pan_changed(QComboBox* cb);
    void on_Start_changed(QComboBox* cb);
    void on_Stop_changed(QComboBox* cb);
    void on_Measure_changed(QComboBox* cb);
    void on_Reverse_changed(QCheckBox* cb);
    void on_LoopFx_changed(QCheckBox* cb);
    void on_OneShot_changed(QCheckBox* cb);
    void on_LoopSync_changed(QCheckBox* cb);
    void on_TempoSync_changed(QCheckBox* cb);
    void on_Input_changed(QComboBox* cb);
    void on_Output_changed(QComboBox* cb);

    // Master Record/Play callbacks
    void on_Master_ComboBox_changed(QComboBox* cb, const char* name);
    void on_Master_CheckBox_changed(QCheckBox* cb, const char* name);
    void on_Master_SpinBox_changed(QSpinBox* sb, const char* name);
    void on_Master_DoubleSpinBox_changed(QDoubleSpinBox* sb, const char* name, int factor);

    // LoopFx
    void on_LoopFx_ComboBox_changed(QComboBox* cb, const char* name);
    void on_LoopFx_CheckBox_changed(QCheckBox* cb, const char* name);
    void on_LoopFx_SpinBox_changed(QSpinBox* sb, const char* name);

    // Rhythm
    void on_Rhythm_ComboBox_changed(QComboBox* cb, const char* name);
    void on_Rhythm_CheckBox_changed(QCheckBox* cb, const char* name);
    void on_Rhythm_Slider_changed(QSlider* s, const char* name);

    // Control
    void on_Control_ComboBox_changed(QComboBox* cb, const char* name);

    void setFilename(const std::string& filename);

private:
    QDialog&        _parent;

    nlohmann::json  _database;
    std::string     _filename;
};
