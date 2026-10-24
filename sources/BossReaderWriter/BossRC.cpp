#include "BossRC.hpp"

#include <fmt/format.h>
#include <pugixml.hpp>

#include <fstream>
#include <stdexcept>
#include <string_view>

namespace {
// --------------------------------------------------------------------------
uint32_t readLE32(const uint8_t* p)
{
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) | (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
}

// --------------------------------------------------------------------------
void writeLE32(uint8_t* p, uint32_t v)
{
    p[0] = static_cast<uint8_t>(v);
    p[1] = static_cast<uint8_t>(v >> 8);
    p[2] = static_cast<uint8_t>(v >> 16);
    p[3] = static_cast<uint8_t>(v >> 24);
}

}   // namespace

namespace BossRC {
// ---------------------------------------------------------------------------
std::string Name::to_string(const int idx) const
{
    return (!name.empty() ? name : fmt::format("Memory{:02}", idx + 1));
}

// ---------------------------------------------------------------------------
std::array<int, 12> Name::to_CXX() const
{
    std::array<int, 12> s = {};

    for (int i = 0; i < s.size(); ++i) {
        s[i] = i < name.size() ? static_cast<int>(name[i]) : ' ';
    }
    return s;
}

// ---------------------------------------------------------------------------
Name Name::From_CXX(const std::array<int, 12>& s)
{
    Name name;
    name.name = "";
    for (const auto i : s) {
        name.name += static_cast<char>(i);
    }
    return name;
}

// --------------------------------------------------------------------------
// Lit l'entier LE32 situé aux 4 derniers octets du fichier.
// --------------------------------------------------------------------------
uint32_t readVersion(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f)
        throw std::runtime_error("Unable to open the file: " + path);

    f.seekg(-4, std::ios::end);
    if (!f)
        throw std::runtime_error("Unable to go to the end of the file");

    uint8_t b[4];
    if (!f.read(reinterpret_cast<char*>(b), 4))
        throw std::runtime_error("Unable to read the last 4 bytes");
    return readLE32(b);
}

// --------------------------------------------------------------------------
// Remplace les 4 derniers octets du fichier par la valeur LE32.
// --------------------------------------------------------------------------
void writeVersion(const std::string& path, const uint32_t version)
{
    std::ofstream f(path, std::ios::binary | std::ios::app);
    if (!f)
        throw std::runtime_error("Unable to open the file: " + path);

    uint8_t b[4] = {};
    writeLE32(b, version);
    f.write(reinterpret_cast<const char*>(b), 4);
}

}   // namespace BossRC
