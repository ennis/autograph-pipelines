cmake_minimum_required(VERSION 3.2)

set(NAME "autograph_main")
file(GLOB SRCS *.cpp)
file(GLOB HDRS *.hpp)

add_executable(${NAME} ${SRCS} ${HDRS})
target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
set_property(TARGET ${NAME} PROPERTY FOLDER "Core")
set_property(TARGET ${NAME} PROPERTY ENABLE_EXPORTS TRUE)
target_link_libraries(
	${NAME} 
	autograph_engine 
	assimp 
	luajit_bin
	sol2 
	fmt 
	glm)
