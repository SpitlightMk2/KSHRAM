set(KSHRAM_plugins)
set(KSHRAM_application_root_dir ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(BasicInsertion)
add_subdirectory(CurveKnob)
add_subdirectory(SmoothCamera)
add_subdirectory(CameraAmp)
add_subdirectory(SVFX)
add_subdirectory(Swing)
add_subdirectory(TiltStyler)

add_subdirectory(CommandBatch)
add_subdirectory(Scripting)

# ===================================== #

add_library( KSHRAM_apps STATIC
    Shared/curves.cpp
    Shared/curve_generator.cpp
    Shared/knob_query.cpp
    
    application_bus.cpp
)

message("Active KSHRAM Plugins:\n ${KSHRAM_plugins}")

target_link_libraries(KSHRAM_apps PRIVATE
    KSHRAM_core
    ${KSHRAM_plugins}
)