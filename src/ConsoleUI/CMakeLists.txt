add_executable(KSHRAM
    console_ui.cpp
    main.cpp
    ${CMAKE_SOURCE_DIR}/icon/icon.rc
)

# 包含versions.h
target_include_directories(KSHRAM PRIVATE ${PROJECT_BINARY_DIR}/src)

target_link_libraries(KSHRAM PRIVATE KSHRAM_core KSHRAM_apps)
set_target_properties(KSHRAM PROPERTIES C_VISIBILITY_PRESET hidden)