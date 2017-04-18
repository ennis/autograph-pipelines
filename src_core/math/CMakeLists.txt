cmake_minimum_required(VERSION 3.2)

set(NAME autograph_math)
file(GLOB SRC *.cpp)

add_library(${NAME} ${SRC})
target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(${NAME} PUBLIC fmt glm)
target_compile_definitions(${NAME} PUBLIC -DAG_MATH_DLL PRIVATE -DAG_BUILD_MATH_DLL)
set_property(TARGET ${NAME} PROPERTY FOLDER "Core")
if (WIN32)
target_compile_options(${NAME} PUBLIC /wd4251)
endif()
