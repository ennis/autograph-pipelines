set(SRC OpenEXR_IO.cpp)
autograph_add_module(PLUGIN NAME OpenEXR_IO SOURCES ${SRC})
target_link_libraries(OpenEXR_IO PUBLIC autographEngine)
target_link_libraries(OpenEXR_IO PRIVATE OpenEXR)
