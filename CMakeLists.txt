cmake_minimum_required(VERSION 3.2)

project(pipelines C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DNOMINMAX -DGLM_FORCE_RADIANS)

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

file(GLOB APP_SOURCES src/*.cpp)
file(GLOB APP_HEADERS src/*.hpp)
source_group("Source files" FILES ${APP_SOURCES})
source_group("Header files" FILES ${APP_HEADERS})

file(GLOB APP_UI_SOURCES src/ui/*.cpp)
file(GLOB APP_UI_HEADERS src/ui/*.hpp)
source_group("Source files\\UI" FILES ${APP_UI_SOURCES})
source_group("Header files\\UI" FILES ${APP_UI_HEADERS})

# generated file names, in the build directory
#set(META_GENERATED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/meta.gen.h")
#set(META_GENERATED_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/meta.gen.cpp")


#====================================================================
#====================================================================
# Reflection parser rules
#====================================================================
#====================================================================

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar ${prefix}${f})
   ENDFOREACH(f)
   SET(${var} ${listVar} PARENT_SCOPE)
MESSAGE("LISTVAR: ${listVar}")
ENDFUNCTION(PREPEND)

function(reflection_gen target)
GET_TARGET_PROPERTY(SRCS ${target} SOURCES)
get_property(INCDIRS TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
MESSAGE("INCDIRS: ${INCDIRS}")
set(META_OUTPUT "meta.gen.cpp")
set(META_OUTPUT_JSON "meta.gen.json")
set(CXX_REFLECTION_TOOL $<TARGET_FILE:cxx-reflect>)
# Don't forget the quotes: https://cmake.org/pipermail/cmake/2015-April/060355.html
# Otherwise the space before -I will be interpreted as an argument separator and will split the generator expression in two!
# Totally intuitive behavior! Bravo CMake developers!
# But wait, there's more! See http://cmake.3232098.n2.nabble.com/Tricky-problem-with-variable-whitespace-quotes-and-shell-td3397636.html
# Fortunately there is a function separate_arguments that you can use!
# Oh no! It doesn't work with generator expressions!
# ...
# Ce genre de 'subtilité' est PARTOUT dans CMake. Pas moyen d'avoir un truc qui marche sans avoir à 
# aller chercher un workaround dans stackoverflow (dans le meilleur des cas) ou tomber sur un bug report.
# Ce n'est pas ACCEPTABLE pour un logiciel de cette taille et avec autant d'utilisateurs. 
# Des fois je me demande si les développeurs de CMake en ont quelque chose à foutre.
SET(INCDIRS2 -I$<JOIN:$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>,\ -I>)
# Workaround: custom addprefix function
# GNU make has it. But not CMake, even if it's otherwise a clusterfuck of fragile and useless features.
# Are they trying to make it intentionally difficult?
# BTW It doesn't work with generator expressions, of course.
#PREPEND(INCDIRS2 -I ${INCDIRS}) 

# Final workaround: do this (and cry)
string (REPLACE ";" " " SRCS_SPACE "${SRCS}")
FILE(GENERATE OUTPUT cxx_reflection_$<CONFIG>.cmake CONTENT "execute_process(COMMAND ${CXX_REFLECTION_TOOL} ${SRCS_SPACE} --output ${META_OUTPUT} --output-jsondb ${META_OUTPUT_JSON} -- -c -std=c++1z -x c++ ${INCDIRS2} OUTPUT_STRIP_TRAILING_WHITESPACE) ")

add_custom_command(
    OUTPUT ${META_OUTPUT}
    DEPENDS ${CXX_REFLECTION_TOOL}
    COMMAND ${CMAKE_COMMAND} -P cxx_reflection_$<CONFIG>.cmake )
target_sources(${target} PUBLIC ${META_OUTPUT})
endfunction()

#====================================================================
#====================================================================
# Executable target
#====================================================================
#====================================================================
add_executable(pipelines 
	${APP_SOURCES} 
	${APP_UI_SOURCES})

reflection_gen(pipelines)

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
