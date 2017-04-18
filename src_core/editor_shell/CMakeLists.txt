cmake_minimum_required(VERSION 3.2)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
qt5_add_resources(EDITOR_RES res/style.qrc)

set(NAME "editor_shell")
file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)
add_executable(${NAME} ${SRCS} ${HDRS} ${EDITOR_RES})
set_property(TARGET ${NAME} PROPERTY ENABLE_EXPORTS TRUE)
target_link_libraries(${NAME} Qt5::Widgets autograph_engine)
set_property(TARGET ${NAME} PROPERTY FOLDER "Tools")
