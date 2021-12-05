add_executable(BossRc500MemoryCopier
    main.cpp
    Dialog.hpp
    DialogBoss.hpp
    DialogBoss.cpp
)

target_link_libraries(BossRc500MemoryCopier PUBLIC Qt6::Qt6)
