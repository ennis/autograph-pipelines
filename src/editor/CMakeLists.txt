# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

add_executable(editor src/main.cpp)

target_link_libraries(editor glfw autograph cppformat json glm nanovg variant)
if(UNIX)
target_link_libraries(editor -lstdc++fs)
endif()
