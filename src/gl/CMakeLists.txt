cmake_minimum_required(VERSION 3.2)

file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.hpp)
find_package(OpenGL REQUIRED)

#====================================================================
#====================================================================
# Main target
#====================================================================
#====================================================================
add_library(autograph_gl SHARED ${LIB_SOURCES})

#====================================================================
#====================================================================
# libraries
#====================================================================
#====================================================================
target_link_libraries(autograph_gl 
	PUBLIC
		autograph_core 
		autograph_support 
	PRIVATE
		${OPENGL_LIBRARIES}
		glm 
		glfw 
		fmt)
target_include_directories(autograph_gl INTERFACE ../)
target_include_directories(autograph_gl PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_compile_definitions(autograph_gl PRIVATE -DAG_GL_DLL -DAG_BUILD_GL_DLL)
target_compile_definitions(autograph_gl INTERFACE -DAG_GL_DLL)

if (WIN32)
target_compile_options(autograph_gl PUBLIC /wd4251)
endif()
