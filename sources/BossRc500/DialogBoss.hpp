#pragma once

#include "Dialog.hpp"

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossCopierDialog : public Ui_BossRc500Dialog, public QObject
{
public:
    explicit BossCopierDialog(QDialog& dialog);

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void on_open();
    void on_copy();
    void on_edit();
    void on_save();
    void on_quit();

    void on_memory_changed();
    void load_memory();

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

private:
    QDialog& _parent;

    nlohmann::json _database;
};
