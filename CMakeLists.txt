cmake_minimum_required(VERSION 3.2)
project(autograph)
set(CMAKE_CXX_STANDARD 11)
set_property( GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(ext/)

file(
	GLOB
	AUTOGRAPH_SOURCES
	src/*.cpp)

file(
	GLOB
	AUTOGRAPH_HEADERS
	src/*.hpp)

include_directories(
	src/)

add_executable(
	autograph
	${AUTOGRAPH_SOURCES}
	${AUTOGRAPH_HEADERS}
	)

target_link_libraries(autograph cppformat glbinding)
target_include_directories(autograph PRIVATE ext/glm)
