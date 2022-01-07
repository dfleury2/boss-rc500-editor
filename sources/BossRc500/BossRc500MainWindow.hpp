#pragma once

#include "Designer/ui_Boss-rc500-main.h"
#include "BossRc500.hpp"

#include <QMainWindow>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>
#include <cmath>

// --------------------------------------------------------------------------
class BossRc500MainWindow : public Ui_MainWindow, public QObject
{
public:
    explicit BossRc500MainWindow(QMainWindow& win);

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void on_ToolMenu_New();
    void on_ToolMenu_Open();
    void on_ToolMenu_Save(bool askDirname = false);
    void on_ToolMenu_PresetSave();
    void on_ToolMenu_PresetLoad(const std::filesystem::path& path);
    void on_ToolMenu_Assign();
    void on_ToolMenu_System();
    void on_ToolMenu_Themes(const std::filesystem::path& path);

    void on_edit();
    void on_copy();

    void on_memory_previous();
    void on_memory_next();
    void on_memory_changed();
    void on_rhythm_play();
    void load_database_mem(const std::string& filename);
    void load_database_sys(const std::string& filename);
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
    void on_LoopFx_FxType_changed();
    void on_LoopFx_ComboBox_changed(QComboBox* cb, const char* name);
    void on_LoopFx_CheckBox_changed(QCheckBox* cb, const char* name);
    void on_LoopFx_SpinBox_changed(QSpinBox* sb, const char* name);

    // Rhythm
    void on_Rhythm_ComboBox_changed(QComboBox* cb, const char* name);
    void on_Rhythm_CheckBox_changed(QCheckBox* cb, const char* name);
    void on_Rhythm_Slider_changed(QSlider* s, const char* name);

    // Control
    void on_Control_ComboBox_changed(QComboBox* cb, const char* name);

    void setDirname(const std::string& dirname);
    void loadPresets();

private:
    template<typename Widget, typename Value>
    void update_mem_database(int memory_index, const char* root, const char* name, Value value, Widget* w)
    {
        std::cout << "Memory: " << (memory_index + 1) << ", " << root << "." << name << ": " << value << std::endl;
        _database_mem["mem"][memory_index][root][name] = static_cast<int>(value);

        auto default_value = BossRc500::DatabaseMemDefault["mem"][memory_index][root][name].get<int>();

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

    template<typename Widget>
    void update_mem_track_database(int memory_index, int track_index, const char* name, int value, Widget* w)
    {
        std::cout << "Memory: " << (memory_index + 1) << ", Track: " << (track_index + 1) << ", " << name << ": " << value << std::endl;
        _database_mem["mem"][memory_index]["TRACK"][track_index][name] = value;

        auto default_value = BossRc500::DatabaseMemDefault["mem"][memory_index]["TRACK"][track_index][name].get<int>();

        bool is_modified = (value != default_value);

        w->setFont(is_modified ? _font_bold : _parent.font());
    }

private:
    QMainWindow&        _parent;

    nlohmann::json  _database_mem;
    nlohmann::json  _database_sys;

    std::string     _dirname;

    QFont _font_bold; // different from default
    QMenu* _presetLoadMenu = nullptr;

};
