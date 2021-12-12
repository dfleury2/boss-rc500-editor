add_executable(xml-reader xml-reader.cpp)

target_link_libraries(xml-reader
    PUBLIC
        rapidxml::rapidxml
        pantor::inja
)
