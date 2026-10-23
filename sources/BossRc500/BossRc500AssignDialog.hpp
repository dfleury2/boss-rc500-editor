#pragma once

#include "Designer/ui_Boss-rc500-assign.h"
#include "BossRc500.hpp"

#include <BossReaderWriter/BossRC.hpp>

#include <spdlog/spdlog.h>

// --------------------------------------------------------------------------
class BossRc500AssignDialog : public Ui_BossRc500AssignDialog, public QObject
{
public:
    explicit BossRc500AssignDialog(QDialog& dialog);

    void load_assign(BossRC::MemoryFile* database, int mem_index, const BossRc500::Beat& beat, int loopFxType, const QString& language);

private:
    void setup();
    void add_tooltips();
    void add_combo_items();
    void add_callbacks();

    void on_target_help();

    void on_Assign_Target_ComboBox_changed(QComboBox* cb, int index);

    template <typename Member>
    void on_Assign_CheckBox_changed(QCheckBox* cb, const int index, Member member)
    {
        update_mem_database(index, member, cb->isChecked(), cb);
    }

    template <typename Member>
    void on_Assign_ComboBox_changed(QComboBox* cb, const int index, Member member)
    {
        update_mem_database(index, member, cb->currentIndex(), cb);
    }

    template<typename Widget, typename Member, typename Value>
    void update_mem_database(const int index, Member member_name, Value value, Widget* w)
    {
        if (!_is_loading) {
            // spdlog::info("Slot:{} - Assign: {} : {}", _memory_index, member_name, value);
            _database->memories[_memory_index].assigns[index].*member_name = value;
        }

        auto default_value = BossRc500::DatabaseMemDefault.memories[_memory_index].assigns[index].*member_name;

        bool is_modified = false;

        if constexpr(std::is_floating_point_v<Value>) {
            is_modified = (std::abs(value - default_value) > 0.05);
        }
        else if constexpr(std::is_same_v<Value, bool>) {
            is_modified = (static_cast<int>(value) != default_value);
        }
        else {
            is_modified = (default_value != value);
        }

        QString styleSheet = QString("font-weight: ") + (is_modified ? "bold" : "normal") + ";";
        w->setStyleSheet(styleSheet);
    }

private:
    QDialog&        _parent;

    BossRC::MemoryFile* _database{nullptr};
    int             _memory_index{0};
    BossRc500::Beat _beat;
    int             _loopFxType{0};
    QString         _language;

    static constexpr int ASSIGN_SIZE = 8;
    std::array<QCheckBox*,ASSIGN_SIZE> _assigns;
    std::array<QComboBox*,ASSIGN_SIZE> _sources;
    std::array<QComboBox*,ASSIGN_SIZE> _sourceModes;
    std::array<QComboBox*,ASSIGN_SIZE> _targets;
    std::array<QComboBox*,ASSIGN_SIZE> _targetMins;
    std::array<QComboBox*,ASSIGN_SIZE> _targetMaxs;

    bool _is_loading = false; // Allow detecting on new/load on callback calls
};
