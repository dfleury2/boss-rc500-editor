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
    void add_callbacks();
    void add_combo_items();

    void on_open();
    void on_copy();
    void on_save();
    void on_quit();

    void on_memory_changed();
    void load_memory();

    // Track callbacks
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

private:
    QDialog& _parent;

    nlohmann::json _database;
};
