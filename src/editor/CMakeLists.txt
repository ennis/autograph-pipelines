add_executable(editor 
	src/main.cpp
	src/entity.cpp
	src/rect_transform.cpp
	src/scene.cpp
	src/input.cpp
	src/coroutine.cpp
	src/debug_imgui.cpp)
reflection_gen(editor src/meta_headers.hpp src/meta.cpp.in)

TARGET_INCLUDE_DIRECTORIES(editor PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(editor glfw autograph cppformat json glm nanovg variant hana imgui optional project_root reflect_lib gsl msgpack
	${Boost_CONTEXT_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${Boost_SYSTEM_LIBRARY})
if(UNIX)
target_link_libraries(editor -lstdc++fs)
endif()

