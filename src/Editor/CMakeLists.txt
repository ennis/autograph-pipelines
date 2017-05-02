cmake_minimum_required(VERSION 3.2)

set(NAME autograph_editor)
file(GLOB SRC *.cpp ui/*.cpp)
add_executable(${NAME} ${SRC})

target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(
		${NAME} 
	PRIVATE
		##### internal
		autograph_support
		autograph_core
		autograph_engine
		##### external
		glfw
		fmt 
		json 
		variant 
		optional   
		gsl 
		zlibstatic
		skia-bin
	PUBLIC
		# GLM and glloadgen are exported as a part of the public API
		glm 
		glloadgen
	)

if(UNIX)
	target_link_libraries(${NAME} PUBLIC -lstdc++fs)
endif()

