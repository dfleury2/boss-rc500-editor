add_library(BossReaderWriter
        BossReaderWriter.hpp
        BossReaderWriter.cpp
)

add_library(BossReaderWriter::BossReaderWriter ALIAS BossReaderWriter)

target_include_directories(BossReaderWriter
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sources>
)

target_link_libraries(BossReaderWriter
    PUBLIC
        nlohmann_json::nlohmann_json
    PRIVATE
        rapidxml::rapidxml
        pantor::inja
)
