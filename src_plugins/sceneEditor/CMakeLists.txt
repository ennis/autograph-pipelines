set(SRC SceneEditor.cpp)
autograph_add_module(PLUGIN NAME SceneEditor SOURCES ${SRC})
target_link_libraries(SceneEditor PUBLIC autographEngine)
