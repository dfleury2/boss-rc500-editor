add_executable(BossRc500
    main.cpp
    BossReaderWriter.hpp
    BossReaderWriter.cpp
    Dialog.hpp
    DialogBoss.hpp
    DialogBoss.cpp
)

target_link_libraries(BossRc500
    PUBLIC
        Qt6::Qt6
        rapidxml::rapidxml
        pantor::inja
        yaml-cpp::yaml-cpp
)
