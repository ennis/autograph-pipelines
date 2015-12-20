cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(autograph CXX)

############## Hack no1 ##############
if(UNIX AND "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -lc++abi")
	include_directories("/usr/include/c++/v1/")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(ext/)

file(
	GLOB
	AUTOGRAPH_SOURCES
	src/*.cpp
	src/backend/opengl/*.cpp)

file(
	GLOB
	AUTOGRAPH_HEADERS
	src/*.hpp
	src/backend/opengl/*.hpp)

include_directories(
	src/)

add_executable(
	autograph
	${AUTOGRAPH_SOURCES}
	${AUTOGRAPH_HEADERS}
	)

target_link_libraries(autograph cppformat glbinding glfw)
target_include_directories(autograph PRIVATE ext/glm)
