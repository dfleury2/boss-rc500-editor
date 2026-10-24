#include "BossRcReaderWriter.hpp"

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
}
