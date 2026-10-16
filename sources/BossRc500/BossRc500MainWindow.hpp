#pragma once

#include "Designer/ui_Boss-rc500-main.h"
#include "BossRc500.hpp"
#include "BossRc500Commands.hpp"
#include "BossRc500AssignDialog.hpp"

#include <QMainWindow>

#include <nlohmann/json.hpp>

#include <iostream>
#include <cmath>

// --------------------------------------------------------------------------
class BossRc500MainWindow : public QMainWindow, public Ui_MainWindow
{
public:
    BossRc500MainWindow();

    void load_memory(int memory_index);
    void set_loading(bool l) { _is_loading = l; }

private:
    void setup();

    void setLogo(const char* boss_version = "500");

    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void on_ToolMenu_New();
    void on_ToolMenu_Open();
    void on_ToolMenu_Save(bool askDirname = false);
    void on_ToolMenu_PresetSave();
    void on_ToolMenu_PresetLoad(const QString& filename);
    void on_ToolMenu_System();
    void on_ToolMenu_Themes(const QString& filename);

    void on_edit();
    void on_copy();
    void on_advanced_copy();

    void on_control_pdlctl_help();

    void on_memory_previous();
    void on_memory_next();
    void on_memory_changed();
    void on_rhythm_play();
    void load_database_mem(const std::string& filename, const std::string& backup = "");
    void load_database_sys(const std::string& filename, const std::string& backup = "");

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
        if (!_is_loading) {
            _stack.push(new UpdateRootName(this, _database_mem, memory_index, root, name, static_cast<int>(value)));
        }

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

        set_modified(w, is_modified);
    }

    template<typename Widget>
    void update_mem_track_database(int memory_index, int track_index, const char* name, int value, Widget* w)
    {
        if (!_is_loading) {
            _stack.push(new UpdateTrackName(this, _database_mem, memory_index, track_index, name, value));
        }

        auto default_value = BossRc500::DatabaseMemDefault["mem"][memory_index]["TRACK"][track_index][name].get<int>();

        set_modified(w, value != default_value);
    }

    void set_modified(QWidget* w, bool is_modified)
    {
        const QString styleSheet = QString("font-weight: ") + (is_modified ? "bold" : "normal") + ";";
        w->setStyleSheet(styleSheet);
    }

    static void
    WriteMemoryDatabase(const nlohmann::json& database, const std::string& filename, int counter);

    static void
    WriteSystemDatabase(const nlohmann::json& database, const std::string& filename, int counter);

    [[nodiscard]] bool isBossRc5Mode() const { return (_database_sys.value("name", "NOT SET") == "RC-5"); }
    [[nodiscard]] bool isBossRc500Mode() const { return !isBossRc5Mode(); }

    static std::pair<std::string, int> GetOldestFile(const std::string& file1, const std::string& file2);

private:
    nlohmann::json  _database_mem;
    nlohmann::json  _database_sys;

    std::string     _dirname;
    std::string     _filename_sys;
    int             _counter_sys_next{};
    std::string     _filename_mem;
    int             _counter_mem_next{};

    QMenu* _presetLoadMenu = nullptr;
    bool _is_loading = false; // Allow detecting on new/load on callback calls

    // Preferences
    QString       _language;
    QActionGroup* _preferences_themes_group{nullptr};
    QActionGroup* _preferences_language_group{nullptr};

    // Assign
    QDialog* _assign_dialog{nullptr};
    BossRc500AssignDialog* _boss_assign_dialog{nullptr};

    // Undo/Redo
    QDialog* _stack_dialog{nullptr};
    QUndoStack _stack;
};
