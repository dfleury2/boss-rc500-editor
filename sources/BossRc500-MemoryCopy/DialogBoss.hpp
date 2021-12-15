#pragma once

#include "Dialog.hpp"

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossRc500MainDialog : public Ui_BossCopierDialog, public QObject
{
public:
    BossRc500MainDialog(QDialog& dialog);

private:
    void setup();

    void on_ToolMenu_Open();
    void on_ToolMenu_Save();
    void on_ToolMenu_Quit();

private:
    QDialog& _parent;

    nlohmann::json _database;
};
