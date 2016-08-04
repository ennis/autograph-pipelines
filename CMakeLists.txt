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
add_subdirectory(src/project_root)
add_subdirectory(src/cxx-reflect)
add_subdirectory(src/autograph)
add_subdirectory(src/editor)
add_subdirectory(src/main)
