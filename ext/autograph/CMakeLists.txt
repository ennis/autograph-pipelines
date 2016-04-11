cmake_minimum_required(VERSION 3.2)

project(autograph CXX)
# For some reason the module path must be absolute.
# This does not seem to be documented (although I didn't search very hard).
# Anyway, f**k you cmake
set(CMAKE_MODULE_PATH "${autograph_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

############## Hack no1 ##############
if(UNIX AND "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -lc++abi")
        #include_directories("/usr/include/c++/v1/")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# TODO if boost is not found, download it and compile it as an external project
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS date_time chrono wave thread filesystem system context REQUIRED)
find_package(Vulkan)
link_directories(${Boost_LIBRARY_DIR})

add_definitions(-DGLM_FORCE_RADIANS)
add_subdirectory(ext/)

file(GLOB SHADERPP_SOURCES src/shaderpp/*.cpp)
file(GLOB SHADERPP_HEADERS src/shaderpp/*.hpp)
source_group("Source files\\shaderpp" FILES ${SHADERPP_SOURCES})
source_group("Header files\\shaderpp" FILES ${SHADERPP_HEADERS})

file(GLOB AG_SOURCES_CORE src/autograph/*.cpp)
source_group("Source files" FILES ${AG_SOURCES_CORE})

file(GLOB AG_HEADERS_CORE src/autograph/*.hpp)
source_group("Header files" FILES ${AG_HEADERS_CORE})

add_library(autograph STATIC ${AG_SOURCES_CORE})

include_directories(src)
include_directories(ext/GSL/include)

#set(AG_INCLUDE_DIR src/)
#set(FILESYSTEM_INCLUDE_DIR ext/filesystem)
#set(GLM_INCLUDE_DIR ext/glm)
#set(GSL_INCLUDE_DIR ext/GSL/include)

add_library(shaderpp STATIC ${SHADERPP_SOURCES})
target_link_libraries(shaderpp ${Boost_LIBRARIES})
target_include_directories(shaderpp PUBLIC ext/GSL/include ${Boost_INCLUDE_DIR})

target_link_libraries(autograph shaderpp glloadgen ${Boost_LIBRARIES} )
target_include_directories(autograph PUBLIC src ext/glm ext/GSL/include)

