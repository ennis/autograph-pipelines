# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5OpenGL)

qt5_add_resources(EDITOR_RES res/style.qrc)

# node editor sources
set(NODE_EDITOR_SRC
        src/node_editor/node.cpp
        src/node_editor/edge.cpp
        src/node_editor/graphicsbezieredge.cpp
        src/node_editor/graphicsnode.cpp
        src/node_editor/graphicsnodeview.cpp
        src/node_editor/graphicsnodescene.cpp
        src/node_editor/graphicsnodesocket.cpp
        src/node_editor/qobjectnode.cpp
)
set(NODE_EDITOR_HEADERS
        src/node_editor/node.hpp
        src/node_editor/edge.hpp
        src/node_editor/graphicsbezieredge.hpp
        src/node_editor/graphicsnode.hpp
        src/node_editor/graphicsnodeview.hpp
        src/node_editor/graphicsnodescene.hpp
        src/node_editor/graphicsnodesocket.hpp
        src/node_editor/graphicsnodedefs.hpp
        src/node_editor/qobjectnode.hpp
)

add_executable(editor src/main.cpp ${EDITOR_RES} ${NODE_EDITOR_SRC})
#reflection_gen(editor src/meta.hpp src/gen/meta.cpp.in)

# Use the Widgets module from Qt 5.
target_link_libraries(editor autograph Qt5::Widgets Qt5::OpenGL cppformat json glm nanovg variant)

