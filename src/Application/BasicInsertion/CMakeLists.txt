set(plugin_name KAHRAM_BasicInsertion)
set(KSHRAM_plugins ${KSHRAM_plugins} ${plugin_name} PARENT_SCOPE)

add_library(${plugin_name} OBJECT
    note_approx.cpp
    knob_approx.cpp
    write_mark.cpp
)

target_include_directories(${plugin_name} PUBLIC ${KSHRAM_application_root_dir})