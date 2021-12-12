add_executable(WidgetMain
    WidgetMain.cpp
    WidgetGallery.hpp
    WidgetGallery.cpp
)

target_link_libraries(WidgetMain
    PUBLIC
        Qt6::Qt6
)
