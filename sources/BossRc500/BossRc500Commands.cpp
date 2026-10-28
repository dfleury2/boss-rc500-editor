#include "BossRc500Commands.hpp"

#include "BossRc500MainWindow.hpp"

#include <spdlog/spdlog.h>

#include <sstream>
#include <utility>

// --------------------------------------------------------------------------
UpdateRootName::UpdateRootName(const std::string& name, BossRc500MainWindow* win, const int index, Function fct, const int value,
                               const int old_value)
    : _name(name), _win(win), _index(index), _fct(std::move(fct)), _value(value), _old_value(old_value)
{
    const auto str = fmt::format("Memory: {}, {} : {}", (_index + 1), name, value);
    setText(str.c_str());
}

// --------------------------------------------------------------------------
void UpdateRootName::updateTo(const int value) const
{
    _fct(value);

    _win->set_loading(true);
    _win->load_memory(_index);
    _win->set_loading(false);
}
// --------------------------------------------------------------------------
UpdateTrackName::UpdateTrackName(const std::string& name, BossRc500MainWindow* win, const int index, const int track, Function fct,
                                 const int value, const int old_value)
    : _name(name), _win(win), _index(index), _track(track), _fct(std::move(fct)), _value(value), _old_value(old_value)
{
    const auto str = fmt::format("Memory: {}, Track{}.{} : {}", (_index + 1), (track + 1), name, value);
    setText(str.c_str());
}

// --------------------------------------------------------------------------
void UpdateTrackName::updateTo(const int value) const
{
    _fct(value);

    _win->set_loading(true);
    _win->load_memory(_index);
    _win->set_loading(false);
}
