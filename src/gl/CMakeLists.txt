cmake_minimum_required(VERSION 3.2)

file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.hpp)

#====================================================================
#====================================================================
# Main target
#====================================================================
#====================================================================
add_library(autograph_gl STATIC ${LIB_SOURCES})

#====================================================================
#====================================================================
# libraries
#====================================================================
#====================================================================
target_link_libraries(autograph_gl autograph_support glm glfw fmt stb mcpp)
target_include_directories(autograph_gl INTERFACE ../)
target_include_directories(autograph_gl PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
