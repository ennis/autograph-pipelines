file(GLOB SRC *.cpp ui/*.cpp)
add_executable(autograph_editor ${SRC})
add_reflection_target(autograph_editor ${AG_PUBLIC_INCLUDE_DIR}/autograph/All.h)

#reflection_gen(editor MetaHeaders.hpp Meta.cpp.in)

target_include_directories(autograph_editor PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(autograph_editor PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(
		autograph_editor 
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
	target_link_libraries(autograph_editor PUBLIC -lstdc++fs)
endif()

