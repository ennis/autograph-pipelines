add_executable(editor 
	src/main.cpp
	src/entity.cpp)

TARGET_INCLUDE_DIRECTORIES(editor PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(editor glfw autograph cppformat json glm nanovg variant hana imgui optional project_root)
if(UNIX)
target_link_libraries(editor -lstdc++fs)
endif()
