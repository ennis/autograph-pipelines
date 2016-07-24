cmake_minimum_required(VERSION 3.2)

file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.hpp)

#====================================================================
#====================================================================
# Main target
#====================================================================
#====================================================================
add_library(autograph STATIC ${LIB_SOURCES})

#====================================================================
#====================================================================
# libraries
#====================================================================
#====================================================================
target_link_libraries(autograph glm assimp glfw cppformat stb mcpp nanovg variant)
target_include_directories(autograph INTERFACE .)

if (WIN32)
target_link_libraries(autograph XInput)
endif()
