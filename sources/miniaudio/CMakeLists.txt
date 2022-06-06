add_library(miniaudio
        miniaudio.h
        miniaudio.c
)

add_library(miniaudio::miniaudio ALIAS miniaudio)

set_target_properties(BossReaderWriter
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(miniaudio
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sources>
)
