add_library(miniaudio
        miniaudio.h
        miniaudio.c
)

add_library(miniaudio::miniaudio ALIAS miniaudio)

target_include_directories(miniaudio
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sources>
)
