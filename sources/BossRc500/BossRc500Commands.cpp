#include "BossRc500Commands.hpp"

#include "BossRc500MainWindow.hpp"

#include <spdlog/spdlog.h>

#include <sstream>

// --------------------------------------------------------------------------
UpdateRootName::UpdateRootName(BossRc500MainWindow* win, nlohmann::json& database,
        int index, const char* root, const char* name, int value) :
    _win(win), _database(database), _index(index), _root(root), _name(name), _value(value)
{
    std::ostringstream oss;
    oss << "Memory: " << (_index + 1) << ", " << _root << "." << _name << ": " << _value;
    setText(oss.str().c_str());
}

// --------------------------------------------------------------------------
void
UpdateRootName::redo()
{
    std::ostringstream oss;
    oss << "Memory: " << (_index + 1) << ", " << _root << "." << _name << ": " << _value;

    auto& mem = _database["mem"][_index][_root][_name];
    _old_value = mem.get<int>();

    mem = _value;

    _win->set_loading(true);
    _win->load_memory(_index);
    _win->set_loading(false);
}

// --------------------------------------------------------------------------
void
UpdateRootName::undo()
{
    std::ostringstream  oss;
    oss << "Memory: " << (_index + 1) << ", " << _root << "." << _name << ": " << _old_value;

    _database["mem"][_index][_root][_name] = _old_value;

    _win->set_loading(true);
    _win->load_memory(_index);
    _win->set_loading(false);
}

// --------------------------------------------------------------------------
UpdateTrackName::UpdateTrackName(BossRc500MainWindow* win, nlohmann::json& database,
        int index, int track, const char* name, int value) :
    _win(win), _database(database), _index(index), _track(track), _name(name), _value(value)
{
    std::ostringstream oss;
    oss << "Memory: " << (_index + 1) << ", Track" << (track + 1) << "." << name << ": " << value;
    setText(oss.str().c_str());
}

// --------------------------------------------------------------------------
void
UpdateTrackName::redo()
{
    spdlog::info("Memory: {}, Track{}.{} : {}", (_index + 1), (_track + 1), _name, _value);

    auto& mem = _database["mem"][_index]["TRACK"][_track][_name];
    _old_value = mem.get<int>();

    mem = _value;

    _win->set_loading(true);
    _win->load_memory(_index);
    _win->set_loading(false);
}

// --------------------------------------------------------------------------
void
UpdateTrackName::undo()
{
    spdlog::info("Memory: {}, Track{}.{} : {}", (_index + 1), (_track + 1), _name, _old_value);

    _database["mem"][_index]["TRACK"][_track][_name] = _old_value;

    _win->set_loading(true);
    _win->load_memory(_index);
    _win->set_loading(false);
}
