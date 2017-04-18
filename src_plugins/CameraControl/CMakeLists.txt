set(SRC CameraControl.cpp)
autograph_add_module(CameraControl ${SRC})
add_dependencies(autograph_main CameraControl)
