#pragma once

#include "BossReaderWriter/BossRC.hpp"

namespace BossRC {
// --------------------------------------------------------------------------
class BossRcReaderWriter
{
  public:
    static SystemFile LoadSystemFile(const std::filesystem::path& p);
    static MemoryFile LoadMemoryFile(const std::filesystem::path& p);

    static void Save(const SystemFile& system_file, const std::filesystem::path& p);
    static void Save(const MemoryFile& memory_file, const std::filesystem::path& p);
};

}   // namespace BossRC
