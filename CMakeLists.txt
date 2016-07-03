cmake_minimum_required(VERSION 3.2)

project(pipelines C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DNOMINMAX -DGLM_FORCE_RADIANS)

find_package(OpenGL REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread filesystem system REQUIRED)

add_subdirectory(ext EXCLUDE_FROM_ALL)
add_subdirectory(tools/cxx-reflect)

file(GLOB APP_SOURCES src/*.cpp)
file(GLOB APP_HEADERS src/*.hpp)
source_group("Source files" FILES ${APP_SOURCES})
source_group("Header files" FILES ${APP_HEADERS})

file(GLOB APP_UI_SOURCES src/ui/*.cpp)
file(GLOB APP_UI_HEADERS src/ui/*.hpp)
source_group("Source files\\UI" FILES ${APP_UI_SOURCES})
source_group("Header files\\UI" FILES ${APP_UI_HEADERS})

# generated file names, in the build directory
set(META_GENERATED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/meta.gen.h")
set(META_GENERATED_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/meta.gen.cpp")

#====================================================================
#====================================================================
# Executable target
#====================================================================
#====================================================================
add_executable(pipelines 
	${APP_SOURCES} 
	${APP_HEADERS} 
	${APP_UI_SOURCES} 
	${APP_UI_HEADERS}
    ${META_GENERATED_HEADER}
    ${META_GENERATED_SOURCE})

#====================================================================
#====================================================================
# Reflection parser rules
#====================================================================
#====================================================================
set(REFLECTION_TOOL_EXE "tools/ReflectionParser.exe")
set(PROJECT_META_HEADER "reflection.h")
# path to the reflection parser executable
set(PARSE_TOOL_EXE "ReflectionParser.exe")

# input source file to pass to the reflection parser compiler
# in this file, include any files that you want exposed to the parser
set(PROJECT_META_HEADER "Reflection.h")

# fetch all include directories for the project target
get_property(DIRECTORIES TARGET ${PROJECT_NAME} PROPERTY INCLUDE_DIRECTORIES)

# flags that will eventually be based to the reflection parser compiler
set(META_FLAGS "")

# build the include directory flags
foreach (DIRECTORY ${DIRECTORIES})
    set(META_FLAGS ${meta_flags} "\\-I${DIRECTORY}")
endforeach ()

# include the system directories
if (MSVC)
    # assume ${VS_VERSION} is the version of Visual Studio being used to compile
    set(META_FLAGS ${META_FLAGS} 
        "\\-IC:/Program Files (x86)/Microsoft Visual Studio 14/VC/include"
    )
else ()
    # you can figure it out for other compilers :)
    message(FATAL_ERROR "System include directories not implemented for this compiler.")
endif ()

# add the command that invokes the reflection parser executable 
# whenever a header file in your project has changed
add_custom_command(
    OUTPUT ${META_GENERATED_HEADER} ${META_GENERATED_SOURCE}
    DEPENDS ${PROJECT_HEADER_FILES}
    COMMAND call "${PARSE_TOOL_EXE}"
    --target-name "${PROJECT_NAME}"
    --in-source "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_SOURCE_DIR}/${PROJECT_META_HEADER}"
    --out-header "${META_GENERATED_HEADER}"
    --out-source "${META_GENERATED_SOURCE}"
    --flags ${META_FLAGS}
)

#====================================================================
#====================================================================
# libraries
#====================================================================
#====================================================================
target_link_libraries(pipelines autograph glm assimp glfw cppformat stb imgui mcpp nanovg variant)

if (WIN32)
target_link_libraries(pipelines XInput)
endif()