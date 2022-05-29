#pragma once

#include <nlohmann/json.hpp>

#include <string>

// --------------------------------------------------------------------------
nlohmann::json
ReadMemoryDatabase(const std::string& filename);

// --------------------------------------------------------------------------
nlohmann::json
ReadSystemDatabase(const std::string& filename);

// --------------------------------------------------------------------------
void
WriteDatabase(const nlohmann::json& database, const std::string& filename,
        const std::string& template_name);
