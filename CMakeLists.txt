cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

project(rc500-tools)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

find_package(inja REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rapidxml REQUIRED)

#set(FLTK_SKIP_FORMS true)
#set(FLTK_SKIP_OPENGL true)
#find_package(FLTK REQUIRED)

add_executable(xml-reader sources/xml-reader.cpp)
target_link_libraries(xml-reader PUBLIC rapidxml::rapidxml pantor::inja)

#add_executable(rc500-reader sources/rc500-reader.cpp)
#target_link_libraries(rc500-reader PUBLIC fltk)
