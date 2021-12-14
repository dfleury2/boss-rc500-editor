#include "BossReaderWriter.hpp"

#include <rapidxml/rapidxml_utils.hpp>
#include <inja/inja.hpp>

#include <fstream>

// --------------------------------------------------------------------------
void
ReadXml(rapidxml::xml_document<>& doc, std::string& buffer, const std::string& filename)
{
    std::ifstream file(filename);

    buffer = std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
    // Remove everything after database end tag (checksum?)
    if (auto end_tag = buffer.find("</database>"); end_tag != std::string::npos) {
        buffer.erase(end_tag + 11);
    }

    doc.parse<0>(buffer.data());
}

// --------------------------------------------------------------------------
nlohmann::json
ExtractFlatXmlStruct(rapidxml::xml_node<>* node)
{
    nlohmann::json flatJson;

    for (auto elem = node->first_node(); elem; elem = elem->next_sibling()) {
        flatJson[elem->name()] = std::stoi(elem->value());
    }

    return flatJson;
}

// --------------------------------------------------------------------------
void
AppendStruct(nlohmann::json& json, rapidxml::xml_node<>* node, const char* name)
{
    if (auto elem = node->first_node(name); elem) {
        json[name] = ExtractFlatXmlStruct(elem);
    }
}

// --------------------------------------------------------------------------
nlohmann::json
ExtractMemory(rapidxml::xml_node<>* node)
{
    nlohmann::json memory;

    if (auto name = node->first_node("NAME"); name) {
        for (auto c = name->first_node(); c; c = c->next_sibling()) {
            memory["NAME"][c->name()] = std::stoi(c->value());
        }
    }

    memory["TRACK"] = nlohmann::json::array();
    int track_number = 0;
    auto track_name = [&track_number] { return "TRACK" + std::to_string(track_number + 1); };

    for (auto track = node->first_node(track_name().c_str()); track; track = track->next_sibling(track_name().c_str())) {
        memory["TRACK"].push_back(ExtractFlatXmlStruct(track));
        ++track_number;
    }

    AppendStruct(memory, node, "MASTER");
    memory["MASTER"]["Tempo"] = memory["MASTER"]["Tempo"].get<int>() / 10.0;

    AppendStruct(memory, node, "LOOPFX");
    AppendStruct(memory, node, "RHYTHM");
    AppendStruct(memory, node, "CTL");

    memory["ASSIGN"] = nlohmann::json::array();
    int assign_number = 0;
    auto assign_name = [&assign_number] { return "ASSIGN" + std::to_string(assign_number + 1); };

    for (auto track = node->first_node(assign_name().c_str()); track; track = track->next_sibling(assign_name().c_str())) {
        memory["ASSIGN"].push_back(ExtractFlatXmlStruct(track));
        (*memory["ASSIGN"].rbegin())["id"] = assign_number;
        ++assign_number;
    }

    return memory;
}

// --------------------------------------------------------------------------
nlohmann::json
ExtractDatabase(rapidxml::xml_document<>& doc)
{
    nlohmann::json database;

    // Reading the database
    auto root = doc.first_node("database");
    if (!root) throw std::runtime_error("No root tag database found");

    database["name"] = root->first_attribute("name")->value();
    database["revision"] = root->first_attribute("revision")->value();

    // mem
    int mem_index = 0;
    database["mem"] = nlohmann::json::array();
    for (auto mem = root->first_node("mem"); mem; mem = mem->next_sibling("mem")) {
        database["mem"].push_back(ExtractMemory(mem));
        (*database["mem"].rbegin())["id"] = mem_index;

        auto mem_name = mem->first_attribute("name");
        (*database["mem"].rbegin())["name"] = (mem_name ? mem_name->value() : "");
        ++mem_index;
    }

    return database;
}

// --------------------------------------------------------------------------
nlohmann::json
ReadMemoryDatabase(const std::string& filename)
{
    // Read Xml File to a database
    std::string buffer;
    rapidxml::xml_document doc;
    ReadXml(doc, buffer, filename);

    return ExtractDatabase(doc);
}

// --------------------------------------------------------------------------
void
WriteMemoryDatabase(const nlohmann::json& database, const std::string& filename)
{
    // Load Inja template for MEMORY file, and render it with database
    inja::Environment env;
    inja::Template tpl = env.parse_file("./resources/templates/MEMORY.txt");

    // Write the output
    std::ofstream file(filename);
    file << env.render(tpl, database);
}
