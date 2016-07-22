# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)

add_executable(editor src/main.cpp)
reflection_gen(editor src/meta.hpp src/gen/meta.cpp.in)

# Use the Widgets module from Qt 5.
target_link_libraries(editor Qt5::Widgets cppformat json glm nanovg variant)

