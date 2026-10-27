#include "BossRcReaderWriter.hpp"

#include "BossRcConnect.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <iostream>

// --------------------------------------------------------------------------
static void dump(const char* t, const std::vector<uint8_t>& v)
{
    fmt::println("{}: {:02X}", t, fmt::join(v, " "));
}

// --------------------------------------------------------------------------
int main(int argc, char** argv)
{
    if (argc > 1) {
        const auto system_file = BossRC::BossRcReaderWriter::LoadSystemFile(argv[1]);

        BossRC::BossRcReaderWriter::Save(system_file, std::string(argv[1]) + ".xml");
    }

    if (argc > 2) {
        const auto memory_file = BossRC::BossRcReaderWriter::LoadMemoryFile(argv[2]);

        BossRC::BossRcReaderWriter::Save(memory_file, std::string(argv[2]) + ".xml");
    }

    dump("edit ON   ", rc500::edit_mode_on());
    dump("edit OFF  ", rc500::edit_mode_off());
    dump("edit query", rc500::edit_mode_query());

#ifdef _WIN32
    rc500::Connection c;
    if (!c.open()) {
        fmt::println("RC-500 introuvable (port MIDI BOSS_RC-500)");
        return 1;
    }
    if (!c.enterEditMode()) {
        fmt::println("edit mode non confirmé");
        return 2;
    }
    std::string drive;
    if (!rc500::Connection::waitForVolume(drive)) {
        fmt::println("volume USB non apparu");
        return 3;
    }
    fmt::println("Connecté : {}", drive);
    fmt::println("Entrée pour déconnecter...");
    std::getchar();

    fmt::println("Unmount volume");
    rc500::Connection::unmountVolume(drive);

    fmt::println("Leave edit mode");
    c.leaveEditMode();

    fmt::println("Done");
#endif
}
