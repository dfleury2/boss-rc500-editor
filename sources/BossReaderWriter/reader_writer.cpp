#include "BossRC.hpp"

int main(int argc, char** argv)
{
    if (argc > 1) {
        const auto system_file = BossRC::SystemFile::load(argv[1]);

        system_file.save(std::string(argv[1]) + ".xml");
    }

    if (argc > 2) {
        const auto memory_file = BossRC::MemoryFile::load(argv[2]);

        memory_file.save(std::string(argv[2]) + ".xml");

    }
}
