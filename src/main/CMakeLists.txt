# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5OpenGL)

qt5_add_resources(EDITOR_RES res/style.qrc)

add_executable(autograph_main 
    src/main.cpp 
    src/Effect.cpp 
    src/Mesh.cpp 
    src/Scene.cpp
    src/SceneRenderer.cpp
    src/Bindings.cpp
    ${EDITOR_RES})
#reflection_gen(editor src/meta.hpp src/gen/meta.cpp.in)

# Use the Widgets module from Qt 5.
target_link_libraries(autograph_main autograph_core autograph_gl autograph_support assimp luajit_bin sol2 Qt5::Widgets Qt5::OpenGL fmt json glm variant)
