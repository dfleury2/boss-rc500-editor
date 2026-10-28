#pragma once

#include <QUndoStack>

// --------------------------------------------------------------------------
class BossRc500MainWindow;

// --------------------------------------------------------------------------
class UpdateRootName : public QUndoCommand
{
  public:
    using Function = std::function<void(int)>;

    UpdateRootName(const std::string& name, BossRc500MainWindow* win, int index, Function fct, int value, int old_value);

    void redo() override { updateTo(_value); }
    void undo() override { updateTo(_old_value); }

  private:
    std::string _name;
    BossRc500MainWindow* _win;

    int _index{};
    Function _fct;

    int _value{};
    int _old_value{};

    void updateTo(int value) const;
};

// --------------------------------------------------------------------------
class UpdateTrackName : public QUndoCommand
{
  public:
    using Function = std::function<void(int)>;

    UpdateTrackName(const std::string& name, BossRc500MainWindow* win, int index, int track, Function fct, int value, int old_value);

    void redo() override { updateTo(_value); }
    void undo() override { updateTo(_old_value); }

  private:
    std::string _name;
    BossRc500MainWindow* _win;

    int _index;
    int _track;
    Function _fct;

    int _value;
    int _old_value{};

    void updateTo(int value) const;
};
