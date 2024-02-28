set(plugin_name KAHRAM_CurveKnob)
set(KSHRAM_plugins ${KSHRAM_plugins} ${plugin_name} PARENT_SCOPE)

add_library(${plugin_name} OBJECT
    curve_knob.cpp
)

target_include_directories(${plugin_name} PUBLIC ${KSHRAM_application_root_dir})