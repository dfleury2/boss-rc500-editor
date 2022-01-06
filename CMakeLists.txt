cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

project(Boss-Rc500-Editor)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT WIN32)
    add_definitions(-DLINUX)
endif()
if(APPLE)
    add_definitions(-DAPPLE)
endif()


find_package(inja REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidxml REQUIRED)
find_package(Qt6 REQUIRED)

add_subdirectory(sources/BossReaderWriter)
add_subdirectory(sources/miniaudio)
add_subdirectory(sources/BossRc500)
#add_subdirectory(sources/BossRc500-MemoryCopy)

# Some widget examples to learn Qt
#add_subdirectory(sources/WidgetExamples)

# Not used, only for first prototyping
#add_subdirectory(sources/xml-reader)

# Installation steps for Resources
install(DIRECTORY resources/drumkits  DESTINATION resources)
install(DIRECTORY resources/images  DESTINATION resources)
install(DIRECTORY resources/templates  DESTINATION resources)
install(DIRECTORY resources/tooltips  DESTINATION resources)
install(DIRECTORY resources/presets  DESTINATION resources)
install(DIRECTORY resources/fonts  DESTINATION resources)
install(DIRECTORY doc  DESTINATION .)

if(WIN32)
    add_custom_target(zip-package
            DEPENDS install
            COMMAND 7z a -tzip BossRc500.zip ${CMAKE_INSTALL_PREFIX}
            )
endif()
