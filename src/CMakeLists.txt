
# 版本信息
configure_file(versions.h.in versions.h @ONLY)

# KSHRAM_core
# 这个部分可能是STL库实现或者Qt实现。
add_library(PathManager OBJECT
    FileSystem/path_manager.cpp
)

add_library(KSHRAM_core STATIC
    misc/enums.cpp
    misc/utilities.cpp
    
    Errors/error_collector.cpp
    

    Entry/entry.cpp
    Measure/measure.cpp
    Header/header.cpp
    Chart/chart.cpp
    Chart/indexed_chart.cpp

    Command/command.cpp
    Command/command_map.cpp
)

target_link_libraries(KSHRAM_core PUBLIC PathManager)

# KSHRAM_apps
add_subdirectory(Application)

if(KSHRAM_QT_MODE)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

    target_compile_definitions(PathManager PRIVATE -DQT_IMPL)
    target_link_libraries(PathManager PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)

    add_subdirectory(QtUI)
else()
    add_subdirectory(ConsoleUI)
endif()

