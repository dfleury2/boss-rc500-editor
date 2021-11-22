cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

project(rc500-tools)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Make this a GUI application on Windows
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
else()
    add_definitions(-DLINUX)
endif()

find_package(inja REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidxml REQUIRED)
find_package(Qt6 REQUIRED)

add_subdirectory(sources/xml-reader)
add_subdirectory(sources/WidgetExamples)
