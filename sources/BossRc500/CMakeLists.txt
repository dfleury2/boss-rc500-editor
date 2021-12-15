# Make this a GUI application on Windows
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

add_executable(BossRc500
        main.cpp
        Designer/Dialog.hpp
        Designer/DialogAssign.hpp
        BossRc500AssignDialog.hpp
        BossRc500AssignDialog.cpp
        BossRc500MainDialog.hpp
        BossRc500MainDialog.cpp
)

target_link_libraries(BossRc500
    PUBLIC
        Qt6::Qt6
        BossReaderWriter::BossReaderWriter
)
