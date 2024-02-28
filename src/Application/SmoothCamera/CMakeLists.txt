set(plugin_name KAHRAM_SmoothCamera)
set(KSHRAM_plugins ${KSHRAM_plugins} ${plugin_name} PARENT_SCOPE)

add_library(${plugin_name} OBJECT
    smooth_camera.cpp
    smooth_camera_increment.cpp
)

target_include_directories(${plugin_name} PUBLIC ${KSHRAM_application_root_dir})