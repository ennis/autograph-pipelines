add_executable(editor 
	src/main.cpp
	src/entity.cpp
	src/rect_transform.cpp
	src/scene.cpp
	src/input.cpp
	src/coroutine.cpp)
reflection_gen(editor src/meta_headers.hpp src/meta.cpp.in)

TARGET_INCLUDE_DIRECTORIES(editor PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(editor glfw autograph cppformat json glm nanovg variant hana imgui optional project_root reflect_lib gsl)
if(UNIX)
target_link_libraries(editor -lstdc++fs)
endif()

