cmake_minimum_required(VERSION 3.12)

project(rc500-tools)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

find_package(inja REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidxml REQUIRED)
find_package(Qt6 REQUIRED)

add_executable(xml-reader sources/xml-reader.cpp)
target_link_libraries(xml-reader PUBLIC rapidxml::rapidxml pantor::inja)

# Make this a GUI application on Windows
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

add_executable(boss-looper
        sources/boss-looper.cpp
        sources/WidgetGallery.hpp
        sources/WidgetGallery.cpp
)

target_link_libraries(boss-looper
    PUBLIC
        rapidxml::rapidxml
        pantor::inja
        Qt6::Qt6
)
