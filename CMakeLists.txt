cmake_minimum_required(VERSION 3.2)

project(pipelines C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DNOMINMAX -DGLM_FORCE_RADIANS)
INCLUDE("cmake/DeployQt5.cmake")

############## Hack no1 ##############
if(UNIX)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++fs")
        #include_directories("/usr/include/c++/v1/")
endif()

find_package(OpenGL REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread filesystem system REQUIRED)

add_subdirectory(ext EXCLUDE_FROM_ALL)
add_subdirectory(tools/cxx-reflect)
add_subdirectory(tools/editor)

file(GLOB APP_SOURCES src/*.cpp)
file(GLOB APP_HEADERS src/*.hpp)
source_group("Source files" FILES ${APP_SOURCES})
source_group("Header files" FILES ${APP_HEADERS})

file(GLOB APP_UI_SOURCES src/ui/*.cpp)
file(GLOB APP_UI_HEADERS src/ui/*.hpp)
source_group("Source files\\UI" FILES ${APP_UI_SOURCES})
source_group("Header files\\UI" FILES ${APP_UI_HEADERS})

FILE(GLOB GEN_SOURCES src/gen/*.cpp.in)

# generated file names, in the build directory
#set(META_GENERATED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/meta.gen.h")
#set(META_GENERATED_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/meta.gen.cpp")

#====================================================================
#====================================================================
# Executable target
#====================================================================
#====================================================================
add_executable(pipelines 
	${APP_SOURCES} 
	${APP_UI_SOURCES})

reflection_gen(pipelines src/meta.hpp ${GEN_SOURCES})

#====================================================================
#====================================================================
# libraries
#====================================================================
#====================================================================
target_link_libraries(pipelines autograph glm assimp glfw cppformat stb imgui mcpp nanovg variant)
target_link_libraries(pipelines -lstdc++fs)

if (WIN32)
target_link_libraries(pipelines XInput)
endif()
