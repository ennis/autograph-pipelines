
set(SRC 
	Application.cpp 
	CVar.cpp
	EntityManager.cpp
	Input.cpp
	ImageUtils.cpp
	DynamicModule.cpp
	ResourceManager.cpp
	Shader.cpp
	Bindings.cpp
	Light.cpp
	Arcball.cpp
	CameraControl.cpp
	RenderUtils.cpp
	SceneObject.cpp
	SceneLoader.cpp
	SceneEditor.cpp
	Mesh.cpp
	ScriptContext.cpp
	RenderTarget.cpp
	ResourcePool.cpp
	Window.cpp
	MathUtils.cpp
	DebugOverlay.cpp
	Renderable.cpp
    ImGuiUtils.cpp
    Meta.cpp
    Profiler.cpp
)

# imgui (with addons)
file(GLOB IMGUI_SOURCES imgui/*.cpp)
add_library(autograph_engine SHARED ${SRC} ${IMGUI_SOURCES})
reflection_generate_database(autograph_engine ${AG_PUBLIC_INCLUDE_DIR}/autograph/All.h)
reflection_render_template(autograph_engine autograph_engine ${CMAKE_CURRENT_SOURCE_DIR}/Meta.cpp.in)
#set_target_properties(autograph_engine PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_include_directories(autograph_engine PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(autograph_engine PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_compile_definitions(autograph_engine PRIVATE -DAG_ENGINE_DLL -DAG_BUILD_ENGINE_DLL)
target_compile_definitions(autograph_engine INTERFACE -DAG_ENGINE_DLL)

if (WIN32)
target_compile_options(autograph_engine PUBLIC /wd4251)
endif()

target_link_libraries(
		autograph_engine 
	PRIVATE
		##### external
		assimp
		zlibstatic
		luajit_bin
		stb
		fmt 
	PUBLIC
		autograph_core	# dynamic
		autograph_support	# dynamic
		autograph_gl # dynamic
		sol2 
		glfw
		json
		variant
		optional
		gsl 
		glm 
	)

if(UNIX)
	target_link_libraries(autograph_engine PUBLIC -lstdc++fs)
endif()


############## Hot-reloadable modules ##############
function(add_autograph_module NAME SOURCES)
    add_library(${NAME} SHARED ${SOURCES})
	target_compile_definitions(${NAME} PRIVATE -DAG_ENGINE_DLL -DAG_GL_DLL -DAG_MODULE)
	target_link_libraries(${NAME} PUBLIC autograph_engine)
	target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
	target_include_directories(${NAME} PUBLIC $<TARGET_PROPERTY:autograph_engine,INCLUDE_DIRECTORIES>)
endfunction()
