#pragma once

#include <QUndoStack>

#include <nlohmann/json.hpp>

class BossRc500MainWindow;

// --------------------------------------------------------------------------
class UpdateRootName : public QUndoCommand {
public:
    explicit UpdateRootName(BossRc500MainWindow* win, nlohmann::json& database,
            int index, const char* root, const char* name, int value);

    void undo();
    void redo();

private:
    BossRc500MainWindow* _win;
    nlohmann::json& _database;
    int _index;
    const char* _root;
    const char* _name;

    int _value;
    int _old_value;
};

// --------------------------------------------------------------------------
class UpdateTrackName : public QUndoCommand {
public:
    explicit UpdateTrackName(BossRc500MainWindow* win, nlohmann::json& database,
            int index, int track, const char* name, int value);

    void undo();
    void redo();

private:
    BossRc500MainWindow* _win;
    nlohmann::json& _database;
    int _index;
    int _track;
    const char* _name;

    int _value;
    int _old_value;
};
