
add_executable(autograph_main 
    main.cpp 
    Effect.cpp 
    Mesh.cpp 
    Scene.cpp
    SceneRenderer.cpp
    Bindings.cpp
	ImGuiBindings.cpp)
#reflection_gen(editor src/meta.hpp src/gen/meta.cpp.in)

target_link_libraries(
	autograph_main 
	autograph_core 
	autograph_gl 
	autograph_support 
	autograph_engine 
	assimp 
	lua53
	sol2 
	fmt 
	glm 
	imgui)
