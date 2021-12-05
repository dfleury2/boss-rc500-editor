add_executable(BossRc500MemoryCopier
    main.cpp
    BossReaderWriter.hpp
    BossReaderWriter.cpp
    Dialog.hpp
    DialogBoss.hpp
    DialogBoss.cpp
)

target_link_libraries(BossRc500MemoryCopier PUBLIC Qt6::Qt6 rapidxml::rapidxml pantor::inja)
