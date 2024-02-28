set(plugin_name KAHRAM_CameraAmp)
set(KSHRAM_plugins ${KSHRAM_plugins} ${plugin_name} PARENT_SCOPE)

add_library(${plugin_name} OBJECT
    camera_amp.cpp
)

target_include_directories(${plugin_name} PUBLIC ${KSHRAM_application_root_dir})