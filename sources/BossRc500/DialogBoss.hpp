#pragma once

#include "Dialog.hpp"

#include <nlohmann/json.hpp>

// --------------------------------------------------------------------------
class BossCopierDialog : public Ui_BossRc500Dialog, public QObject
{
public:
    BossCopierDialog(QDialog& dialog);

private:
    void setup();

    void on_open();
    void on_save();
    void on_quit();

    void on_memory_changed();
    void load_memory();


private:
    QDialog& _parent;

    nlohmann::json _database;
};
