#pragma once

#include "BossRc500.hpp"
#include "BossRc500AssignDialog.hpp"
#include "BossRc500Commands.hpp"
#include "Designer/ui_Boss-rc500-main.h"

#include <BossReaderWriter/BossRC.hpp>

#include <QMainWindow>

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossRc500MainWindow : public QMainWindow, public Ui_MainWindow
{
  public:
    BossRc500MainWindow();

    void load_memory(int memory_index);
    void set_loading(bool l) { _is_loading = l; }

  private:
    void setup();

    void setLogo(const char* boss_version = "500") const;

    void add_tooltips() const;
    void add_combo_items() const;
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

    void on_memory_previous() const;
    void on_memory_next() const;
    void on_memory_changed();
    void on_rhythm_play();
    void load_database_mem(const std::string& filename);
    void load_database_sys(const std::string& filename);

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

    int getTrackId(const QWidget* w) const { return w->parentWidget() == gp_Track1 ? 0 : 1; }

    // Master Record/Play callbacks
    template <typename Member>
    void on_Master_ComboBox_changed(QComboBox* cb, Member member);

    template <typename Member>
    void on_Master_CheckBox_changed(QCheckBox* cb, Member member);

    template <typename Member>
    void on_Master_SpinBox_changed(QSpinBox* sb, Member member);

    template <typename Member>
    void on_Master_DoubleSpinBox_changed(QDoubleSpinBox* sb, Member member, int factor);

    // LoopFx
    void on_LoopFx_FxType_changed();

    template <typename Member>
    void on_LoopFx_ComboBox_changed(QComboBox* cb, Member member);

    template <typename Member>
    void on_LoopFx_CheckBox_changed(QCheckBox* cb, Member member);

    template <typename Member>
    void on_LoopFx_SpinBox_changed(QSpinBox* sb, Member member);

    // Rhythm
    template <typename Member>
    void on_Rhythm_ComboBox_changed(QComboBox* cb, Member name);

    template <typename Member>
    void on_Rhythm_CheckBox_changed(QCheckBox* cb, Member name);

    template <typename Member>
    void on_Rhythm_Slider_changed(QSlider* s, Member name);

    // Control
    template <typename Member>
    void on_Control_ComboBox_changed(QComboBox* cb, Member member);

    void setDirname(const std::string& dirname);
    void loadPresets();

  private:
    template <typename Widget, typename Node, typename Member, typename Value>
    void update_mem_database(const int memory_index, Node node, Member member, Value value, Widget* w)
    {
        if (!_is_loading) {
            // TODO _stack.push(new UpdateRootName(this, _database_memory, memory_index, root, name, static_cast<int>(value)));
        }

        const auto default_value = BossRc500::DatabaseMemDefault.memories[memory_index].*node.*member;

        bool is_modified = false;

        if constexpr (std::is_floating_point_v<Value>) {
            is_modified = (std::abs(value - default_value) > 0.05);
        }
        else if constexpr (std::is_same_v<Value, bool>) {
            is_modified = (static_cast<int>(value) != default_value);
        }
        else {
            is_modified = (default_value != value);
        }

        set_modified(w, is_modified);
    }

    template <typename Widget, typename Member>
    void update_mem_track_database2(const int memory_index, const int track_index, Member member, const int value, Widget* w)
    {
        if (!_is_loading) {
            // TODO _stack.push(new UpdateTrackName(this, _database_memory, memory_index, track_index, name, value));
        }

        const auto default_value = BossRc500::DatabaseMemDefault.memories[memory_index].tracks[track_index].*member;

        set_modified(w, value != default_value);
    }

    static void set_modified(QWidget* w, const bool is_modified)
    {
        const auto styleSheet = fmt::format("font-weight: {};", (is_modified ? "bold" : "normal"));
        w->setStyleSheet(styleSheet.c_str());
    }

    [[nodiscard]] bool isBossRc5Mode() const { return (_database_system.database_name == "RC-5"); }
    [[nodiscard]] bool isBossRc500Mode() const { return !isBossRc5Mode(); }

    static std::string GetOldestFile(const std::string& file1, const std::string& file2);

  private:
    BossRC::SystemFile _database_system;
    BossRC::MemoryFile _database_memory;

    std::string _dirname;
    std::string _filename_sys;
    std::string _filename_mem;

    QMenu* _presetLoadMenu = nullptr;
    bool _is_loading = false;   // Allow detecting on new/load on callback calls

    // Preferences
    QString _language;
    QActionGroup* _preferences_themes_group{nullptr};
    QActionGroup* _preferences_language_group{nullptr};

    // Assign
    QDialog* _assign_dialog{nullptr};
    BossRc500AssignDialog* _boss_assign_dialog{nullptr};

    // Undo/Redo
    QDialog* _stack_dialog{nullptr};
    QUndoStack _stack;
};
