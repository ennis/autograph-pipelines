
set(SRC 
	Application.cpp 
	Input.cpp
	ImageUtils.cpp
	ResourceManager.cpp
	Shader.cpp
	Bindings.cpp
	Arcball.cpp
	CameraControl.cpp
	RenderUtils.cpp
	Scene.cpp
	SceneUtils.cpp
	Mesh.cpp
	ScriptContext.cpp
	RenderTarget.cpp
	ResourcePool.cpp
	Window.cpp
	MathUtils.cpp
	DebugOverlay.cpp
        ImGuiUtils.cpp
        Meta.cpp
)
	
add_library(autograph_engine STATIC ${SRC} )
#reflection_generate_database(autograph_engine ${AG_PUBLIC_INCLUDE_DIR}/autograph/All.h)
#reflection_render_template(autograph_engine autograph_engine ${CMAKE_CURRENT_SOURCE_DIR}/Meta.cpp.in)

target_include_directories(autograph_engine PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(autograph_engine PUBLIC ${AG_PUBLIC_INCLUDE_DIR})

target_link_libraries(
		autograph_engine 
	PRIVATE
		##### internal
		autograph_support
		##### external
		assimp
		glfw
		fmt 
		json 
		variant 
		optional   
		gsl 
		zlibstatic
		imgui
		luajit_bin
		stb
	PUBLIC
		# GLM and glloadgen are exported as a part of the public API
		autograph_gl
		sol2 
		glm 
		glloadgen
		skia-bin
	)

if(UNIX)
	target_link_libraries(autograph_engine PUBLIC -lstdc++fs)
endif()

