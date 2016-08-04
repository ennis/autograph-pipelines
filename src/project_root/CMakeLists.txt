cmake_minimum_required(VERSION 3.2)
add_library(project_root STATIC project_root.cpp)
target_include_directories(project_root INTERFACE .)
target_link_libraries(project_root cppformat)
