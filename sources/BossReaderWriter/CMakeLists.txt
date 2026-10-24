add_library(BossReaderWriter
        BossRcReaderWriter.hpp
        BossRcReaderWriter.cpp
        BossRC.hpp
        BossRC.cpp
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
        pugixml::pugixml
        fmt::fmt
)


add_executable(reader_writer reader_writer.cpp)

target_link_libraries(reader_writer
        PUBLIC
        BossReaderWriter::BossReaderWriter
)

set_target_properties(reader_writer
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
