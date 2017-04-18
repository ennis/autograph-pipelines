cmake_minimum_required(VERSION 3.2)

set(NAME autograph_engine)
file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.hpp)
file(GLOB IMGUI_SOURCES imgui/*.cpp)

add_library(${NAME} SHARED ${LIB_SOURCES} ${LIB_HEADERS} ${IMGUI_SOURCES})
#reflection_generate_database(${NAME} ${AG_PUBLIC_INCLUDE_DIR}/autograph/All.h)
#reflection_render_template(${NAME} ${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/Meta.cpp.in)
#set_target_properties(${NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_compile_definitions(${NAME} PUBLIC -DAG_ENGINE_DLL PRIVATE -DAG_BUILD_ENGINE_DLL)
set_property(TARGET ${NAME} PROPERTY FOLDER "Core")

if (WIN32)
target_compile_options(${NAME} PUBLIC /wd4251 /wd4275)
endif()

target_link_libraries(
		autograph_engine 
	PRIVATE
		##### external
		zlibstatic
		luajit_bin
		stb
		fmt 
	PUBLIC
		autograph_math	# dynamic
		autograph_support	# dynamic
		autograph_gl # dynamic
		sol2 
		glfw
		json
		variant
		optional
		gsl 
		glm 
		OpenEXR
	)

if(UNIX)
	target_link_libraries(autograph_engine PUBLIC -lstdc++fs ${CMAKE_DL_LIBS})
endif()


