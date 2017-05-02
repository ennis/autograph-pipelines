set(SRC SceneLoader.cpp)
autograph_add_module(PLUGIN NAME SceneLoader SOURCES ${SRC})
target_link_libraries(SceneLoader PUBLIC autographEngine PRIVATE assimp)
