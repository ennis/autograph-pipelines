set(SRC SceneLoader.cpp)
add_autograph_module(SceneLoader ${SRC})
target_link_libraries(SceneLoader PRIVATE assimp)
