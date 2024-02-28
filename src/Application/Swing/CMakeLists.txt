set(plugin_name KAHRAM_Swing)
set(KSHRAM_plugins ${KSHRAM_plugins} ${plugin_name} PARENT_SCOPE)

add_library(${plugin_name} OBJECT
    swing.cpp
)

target_include_directories(${plugin_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})