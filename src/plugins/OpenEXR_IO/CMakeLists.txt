set(SRC OpenEXR_IO.cpp)
add_autograph_module(OpenEXR_IO ${SRC})
target_link_libraries(OpenEXR_IO PRIVATE IlmImf)
