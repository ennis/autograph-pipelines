cmake_minimum_required(VERSION 3.2)

set(NAME autograph_gl)
file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.hpp)
find_package(OpenGL REQUIRED)

#====================================================================
#====================================================================
# Main target
#====================================================================
#====================================================================
add_library(${NAME} SHARED ${LIB_SOURCES})

#====================================================================
#====================================================================
# libraries
#====================================================================
#====================================================================
target_link_libraries(${NAME} 
	PUBLIC
		autograph_math 
		autograph_support 
	PRIVATE
		${OPENGL_LIBRARIES}
		glm 
		glfw 
		fmt)
target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_compile_definitions(${NAME} PUBLIC -DAG_GL_DLL PRIVATE -DAG_BUILD_GL_DLL)
set_property(TARGET ${NAME} PROPERTY FOLDER "Core")

if (WIN32)
target_compile_options(${NAME} PUBLIC /wd4251)
endif()
