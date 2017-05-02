set(SRC CameraControl.cpp)
autograph_add_module(PLUGIN NAME CameraControl SOURCES ${SRC})
target_link_libraries(CameraControl PUBLIC autographEngine)
add_dependencies(autographMain CameraControl)
