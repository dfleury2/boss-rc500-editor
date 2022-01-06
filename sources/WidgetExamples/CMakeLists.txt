add_executable(WidgetMain
    WidgetMain.cpp
    WidgetGallery.hpp
    WidgetGallery.cpp
)

target_link_libraries(WidgetMain
    PUBLIC
        Qt6::Qt6
)

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-framework IOSurface -framework CoreVideo -framework Carbon -framework Metal -framework QuartzCore")
endif()
