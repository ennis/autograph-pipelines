
set(SRC main.cpp 
    Effect.cpp 
    Mesh.cpp 
    Scene.cpp
    SceneRenderer.cpp
	ShaderManager.cpp
    Bindings.cpp
	CameraControl.cpp
	Arcball.cpp)

set(HEADERS 
	Arcball.h
	CameraControl.h
	Bindings.h
	Effect.h
	Mesh.h
	Scene.h
	SceneRenderer.h
	ShaderManager.h)

add_executable(autograph_main 
    ${SRC} ${HEADERS})
#reflection_gen(editor src/meta.hpp src/gen/meta.cpp.in)

target_link_libraries(
	autograph_main 
	autograph_core 
	autograph_gl 
	autograph_support 
	autograph_engine 
	assimp 
	luajit_bin
	sol2 
	fmt 
	glm 
	imgui)
