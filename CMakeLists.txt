cmake_minimum_required(VERSION 3.2)

project(pipelines C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-DNOMINMAX -DGLM_FORCE_RADIANS -D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(AG_PUBLIC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(AG_VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(AG_VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(AG_VERSION_PATCH   1   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

include("cmake/ReflectionGen.cmake")

############## Doxygen ##############
# https://tty1.net/blog/2014/cmake-doxygen_en.html
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()


############## Hack no1 ##############
if(UNIX)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++fs")
        #include_directories("/usr/include/c++/v1/")
endif()

############## Force clang modules ##############
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules -fmodules-cache-path=${CMAKE_SOURCE_DIR}/cache")

find_package(OpenGL REQUIRED)
#find_package(Mono REQUIRED)
link_directories(${MONO_LIBRARY_DIRS})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread filesystem system context REQUIRED)

add_subdirectory(ext EXCLUDE_FROM_ALL)
add_subdirectory(src/core)
add_subdirectory(src/support)
add_subdirectory(src/gl)
add_subdirectory(src/editor)
add_subdirectory(src/cxx-reflect)
add_subdirectory(src/main)
