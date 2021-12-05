#pragma once

#include "Dialog.hpp"

// --------------------------------------------------------------------------
class BossCopierDialog : public Ui_BossCopierDialog, public QObject
{
public:
    BossCopierDialog(QDialog& dialog);

private:
    void setup();

    void on_open();
    void on_save();
    void on_quit();

private:
    QDialog& _parent;
};
