# Make this a GUI application on Windows
if (WIN32)
    #set(CMAKE_WIN32_EXECUTABLE ON)
endif ()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(app_icon_resource_windows "${PROJECT_SOURCE_DIR}/resources/images/BossRc500.rc")

add_executable(BossRc500
        Designer/Boss-rc500-main.ui
        Designer/Boss-rc500-assign.ui
        Designer/Boss-rc500-system.ui
        Designer/Boss-rc500-text.ui
        Designer/Boss-rc500-copy.ui
        main.cpp
        BossRc500.hpp
        BossRc500.cpp
        BossRc500AdvancedCopyDialog.hpp
        BossRc500AdvancedCopyDialog.cpp
        BossRc500AssignDialog.cpp
        BossRc500SystemDialog.hpp
        BossRc500SystemDialog.cpp
        BossRc500MainWindow.hpp
        BossRc500MainWindow.cpp
        BossRc500Commands.hpp
        BossRc500Commands.cpp
        ${app_icon_resource_windows}
)

set_target_properties(BossRc500
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(BossRc500
    PUBLIC
        qt::qt
        BossReaderWriter::BossReaderWriter
        miniaudio::miniaudio
        nlohmann_json::nlohmann_json
        yaml-cpp
        pantor::inja
)

if(WIN32)
target_link_libraries(BossRc500
    PUBLIC
        DWrite.lib
        Shcore.lib
        D3d9.lib
)
endif()

install(TARGETS BossRc500
    RUNTIME DESTINATION .
)
