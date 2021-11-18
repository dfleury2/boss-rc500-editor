#include <nlohmann/json.hpp>
#include <rapidxml_print.hpp>
#include <fmt/format.h>
#include <inja/inja.hpp>

#include <iostream>
#include <fstream>
#include <string>

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
template<typename T = std::string>
T
GetAttribute(rapidxml::xml_node<>* node, const char* name)
{
    T t{};

    auto attr = node->first_attribute(name);
    if (!attr) throw std::runtime_error("Attribute [" + std::string(name) + "]");

    if constexpr(std::is_integral_v<T>) {
        t = std::atoi(attr->value());
    }
    else {
        t = attr->value();
    }

    return t;
}

// --------------------------------------------------------------------------
nlohmann::json
ExtractFlatStruct(rapidxml::xml_node<>* node)
{
    nlohmann::json track;

    for (auto elem = node->first_node(); elem; elem = elem->next_sibling()) {
        track[elem->name()] = std::stoi(elem->value());
    }

    return track;
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
    auto track_name = [&track_number] { return fmt::format("TRACK{}", track_number + 1); };

    for (auto track = node->first_node(track_name().c_str()); track; track = track->next_sibling(track_name().c_str())) {
        memory["TRACK"].push_back(ExtractFlatStruct(track));
        (*memory["TRACK"].rbegin())["id"] = track_number;
        ++track_number;
    }

    if (auto master = node->first_node("MASTER"); master) {
        memory["MASTER"] = ExtractFlatStruct(master);
    }

    if (auto loopFx = node->first_node("LOOPFX"); loopFx) {
        memory["LOOPFX"] = ExtractFlatStruct(loopFx);
    }

    if (auto rhythm = node->first_node("RHYTHM"); rhythm) {
        memory["RHYTHM"] = ExtractFlatStruct(rhythm);
    }

    if (auto ctl = node->first_node("CTL"); ctl) {
        memory["CTL"] = ExtractFlatStruct(ctl);
    }

    memory["ASSIGN"] = nlohmann::json::array();
    int assign_number = 0;
    auto assign_name = [&assign_number] { return fmt::format("ASSIGN{}", assign_number + 1); };

    for (auto track = node->first_node(assign_name().c_str()); track; track = track->next_sibling(assign_name().c_str())) {
        memory["ASSIGN"].push_back(ExtractFlatStruct(track));
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

    database["name"] = GetAttribute<std::string>(root, "name");
    database["revision"] = GetAttribute(root, "revision");

    // mem
    int mem_index = 0;
    database["mem"] = nlohmann::json::array();
    for (auto mem = root->first_node("mem"); mem; mem = mem->next_sibling("mem")) {
        database["mem"].push_back(ExtractMemory(mem));
        (*database["mem"].rbegin())["id"] = mem_index;
        ++mem_index;
    }

    return database;
}

// --------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    try {
        if (argc < 3) throw std::runtime_error("Usage: xml-reade <xml file> <inja file>");

        // Read Xml File to a database
        std::string buffer;
        rapidxml::xml_document doc;
        ReadXml(doc, buffer, argv[1]);

        auto database = ExtractDatabase(doc);

        // Load Inja template for MEMORY file, and render it with database
        inja::Environment env;
        inja::Template tpl = env.parse_file(argv[2]);

        auto output = env.render(tpl, database);

        // Write the output
        std::cout << output << std::endl;

    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}
