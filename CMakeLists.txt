cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

project(rc500-tools)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT WIN32)
    add_definitions(-DLINUX)
endif()

find_package(inja REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidxml REQUIRED)
find_package(Qt6 REQUIRED)
find_package(yaml-cpp REQUIRED)

add_subdirectory(sources/BossReaderWriter)
add_subdirectory(sources/BossRc500)
add_subdirectory(sources/BossRc500-MemoryCopy)

# Some widget examples to learn Qt
#add_subdirectory(sources/WidgetExamples)

# Not used, only for first prototyping
#add_subdirectory(sources/xml-reader)
