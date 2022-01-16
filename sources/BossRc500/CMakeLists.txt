# Make this a GUI application on Windows
if (WIN32)
    #set(CMAKE_WIN32_EXECUTABLE ON)
endif ()

set(app_icon_resource_windows "${PROJECT_SOURCE_DIR}/resources/images/BossRc500.rc")

qt_add_executable(BossRc500
        Designer/ui_Boss-rc500-main.h
        Designer/ui_Boss-rc500-assign.h
        Designer/ui_Boss-rc500-system.h
        Designer/ui_Boss-rc500-text.h
        Designer/ui_Boss-rc500-copy.h
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
        ${app_icon_resource_windows}
)

target_link_libraries(BossRc500
    PUBLIC
        Qt6::Qt6
        BossReaderWriter::BossReaderWriter
        miniaudio::miniaudio
        nlohmann_json::nlohmann_json
        yaml-cpp::yaml-cpp
        pantor::inja
)

install(TARGETS BossRc500
        RUNTIME DESTINATION .
)
