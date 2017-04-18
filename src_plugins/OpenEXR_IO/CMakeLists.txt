set(SRC OpenEXR_IO.cpp)
autograph_add_module(OpenEXR_IO ${SRC})
target_link_libraries(OpenEXR_IO PRIVATE OpenEXR)
