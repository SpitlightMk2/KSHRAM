set(plugin_name KAHRAM_TiltStyler)
set(KSHRAM_plugins ${KSHRAM_plugins} ${plugin_name} PARENT_SCOPE)

add_library(${plugin_name} OBJECT
    tilt_guide.cpp
    tilt_style.cpp
    style_config.cpp
    tilt_styler.cpp
)

target_include_directories(${plugin_name} PUBLIC ${KSHRAM_application_root_dir})