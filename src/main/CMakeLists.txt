
set(SRC main.cpp 
    SceneRenderer.cpp)

set(HEADERS 
	SceneRenderer.h)

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
