cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

project(Boss-Rc500-Editor VERSION 0.0.4)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT WIN32)
    add_definitions(-DLINUX)
endif()
if(APPLE)
    add_definitions(-DAPPLE)
    # Waiting for something better
    set(CMAKE_EXE_LINKER_FLAGS "-framework IOSurface -framework CoreVideo -framework Carbon -framework Metal -framework QuartzCore")
endif()


find_package(inja REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidxml REQUIRED)
find_package(Qt6 REQUIRED)
find_package(yaml-cpp REQUIRED)

add_subdirectory(sources/BossReaderWriter)
add_subdirectory(sources/miniaudio)
add_subdirectory(sources/BossRc500)

# Installation steps for Resources
install(DIRECTORY resources/drumkits  DESTINATION resources)
install(DIRECTORY resources/images  DESTINATION resources)
install(DIRECTORY resources/themes  DESTINATION resources)
install(DIRECTORY resources/templates  DESTINATION resources)
install(DIRECTORY resources/tooltips  DESTINATION resources)
install(DIRECTORY resources/presets  DESTINATION resources)
install(DIRECTORY resources/fonts  DESTINATION resources)
install(DIRECTORY doc  DESTINATION .)


# Packaging
set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/doc/BossRC500.png")

include(CPack)
