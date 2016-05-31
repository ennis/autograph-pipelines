cmake_minimum_required(VERSION 3.2)

project(pipelines CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DNOMINMAX -DGLM_FORCE_RADIANS)

find_package(OpenGL REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread filesystem system REQUIRED)

add_subdirectory(ext EXCLUDE_FROM_ALL)

file(GLOB APP_SOURCES src/*.cpp)
file(GLOB APP_HEADERS src/*.hpp)
source_group("Source files" FILES ${APP_SOURCES})
source_group("Header files" FILES ${APP_HEADERS})

file(GLOB APP_UI_SOURCES src/ui/*.cpp)
file(GLOB APP_UI_HEADERS src/ui/*.hpp)
source_group("Source files\\UI" FILES ${APP_UI_SOURCES})
source_group("Header files\\UI" FILES ${APP_UI_HEADERS})


add_executable(pipelines ${APP_SOURCES} ${APP_HEADERS} ${APP_UI_SOURCES} ${APP_UI_HEADERS})
target_link_libraries(pipelines autograph glm assimp glfw cppformat stb imgui mcpp nanovg variant)

if (WIN32)
target_link_libraries(pipelines XInput)
endif()