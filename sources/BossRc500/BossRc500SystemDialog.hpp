#pragma once

#include "BossRc500.hpp"
#include "Designer/ui_Boss-rc500-system.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

// --------------------------------------------------------------------------
class BossRc500SystemDialog : public Ui_BossRc500SystemDialog, public QObject
{
  public:
    explicit BossRc500SystemDialog(QDialog& dialog, const BossRC::SystemFile& database, const QString& language);

    bool apply = false;
    BossRC::SystemFile database;

  private:
    void setup();
    void add_tooltips() const;
    void add_combo_items();
    void add_callbacks();

    void load_system();

    void on_control_pdlctl_help();

    template <typename Node, typename Member>
    void on_System_ComboBox_changed(QComboBox* cb, Node node, Member member)
    {
        update_sys_database(node, member, cb->currentIndex(), cb);
    }

    template <typename Node, typename Member>
    void on_System_CheckBox_changed(QCheckBox* cb, Node node, Member member)
    {
        update_sys_database(node, member, cb->isChecked(), cb);
    }

    template <typename Node, typename Member>
    void on_System_SpinBox_changed(QSpinBox* cb, Node node, Member member)
    {
        update_sys_database(node, member, cb->value(), cb);
    }

    template <typename Widget, typename Node, typename Member, typename Value>
    void update_sys_database(Node root, Member member, Value value, Widget* w)
    {
        if (!_is_loading) {
            // spdlog::info("System: {}.{}: {}", root, member_name, value);
            database.*root.*member = value;
        }

        auto default_value = BossRc500::DatabaseSysDefault.*root.*member;

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

        QString styleSheet = QString("font-weight: ") + (is_modified ? "bold" : "normal") + ";";
        w->setStyleSheet(styleSheet);
    }

  private:
    QDialog& _parent;
    QString _language;

    bool _is_loading = false;   // Allow detecting on new/load on callback calls
};
