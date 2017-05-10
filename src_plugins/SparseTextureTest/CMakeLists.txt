set(SRC SparseTextureTest.cpp)
autograph_add_module(PLUGIN NAME SparseTextureTest SOURCES ${SRC})
target_link_libraries(SparseTextureTest PUBLIC autographEngine)
