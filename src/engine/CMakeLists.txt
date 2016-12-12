add_library(autograph_engine STATIC
	Application.cpp 
	Input.cpp)
add_reflection_target(autograph_engine ${AG_PUBLIC_INCLUDE_DIR}/autograph/All.h)
target_include_directories(autograph_engine PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(autograph_engine PUBLIC ${AG_PUBLIC_INCLUDE_DIR})

target_link_libraries(
		autograph_engine 
	PRIVATE
		##### internal
		autograph_support
		##### external
		glfw
		fmt 
		json 
		variant 
		optional   
		gsl 
		zlibstatic
		imgui
	PUBLIC
		# GLM and glloadgen are exported as a part of the public API
		glm 
		glloadgen
		skia-bin
	)

if(UNIX)
	target_link_libraries(autograph_engine PUBLIC -lstdc++fs)
endif()

