set(SRC DeferredSceneRenderer.cpp)
autograph_add_module(PLUGIN NAME DeferredSceneRenderer SOURCES ${SRC})
target_link_libraries(DeferredSceneRenderer PUBLIC autographEngine)
