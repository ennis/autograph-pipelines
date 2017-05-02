find_package(Qt5Widgets)
set(SRC CurveEditor.cpp)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
autograph_add_module(PLUGIN NAME CurveEditor SOURCES ${SRC})
target_link_libraries(CurveEditor PUBLIC Qt5::Widgets autographEngine)
