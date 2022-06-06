add_library(BossReaderWriter
        BossReaderWriter.hpp
        BossReaderWriter.cpp
)

add_library(BossReaderWriter::BossReaderWriter ALIAS BossReaderWriter)

set_target_properties(BossReaderWriter
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

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
