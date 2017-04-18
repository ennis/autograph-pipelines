set(SRC SceneLoader.cpp)
autograph_add_module(SceneLoader ${SRC})
target_link_libraries(SceneLoader PRIVATE assimp)
#add_dependencies(autograph_main SceneLoader)
