# Make this a GUI application on Windows
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

add_executable(BossRc500MemoryCopier
        main.cpp
        Dialog.hpp
        DialogBoss.hpp
        DialogBoss.cpp
)

target_link_libraries(BossRc500MemoryCopier
    PUBLIC
        Qt6::Qt6
        BossReaderWriter::BossReaderWriter
)
