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

FILE(GLOB GEN_SOURCES src/gen/*.cpp.in)

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
ENDFUNCTION(PREPEND)

FUNCTION(PREP_GENERATED_SOURCE_FILES VAR BINDIR)
   SET(listVar "")
   FOREACH(f ${ARGN}) 
       # NAME_WE strips the 'longest extension'. HAHAHAHAHAHAAHAHA
	   GET_FILENAME_COMPONENT(FILENAME ${f} NAME)
	  string(REGEX REPLACE "\\.[^.]*$" "" STEM ${FILENAME})
      LIST(APPEND listVar  ${BINDIR}/${STEM})
   ENDFOREACH(f)
   SET(${VAR} ${listVar} PARENT_SCOPE)
ENDFUNCTION(PREP_GENERATED_SOURCE_FILES)


function(reflection_gen TARGET TEMPLATES)
GET_TARGET_PROPERTY(SRCS ${TARGET} SOURCES)
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
SET(INCDIRS2 -I$<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>,\ -I>)
# Workaround: custom addprefix function
# GNU make has it. But not CMake, even if it's otherwise a clusterfuck of fragile and useless features.
# Are they trying to make it intentionally difficult?
# BTW It doesn't work with generator expressions, of course.
#PREPEND(INCDIRS2 -I ${INCDIRS}) 

set(CL_OUTPUT_JSON "meta.gen.json")
PREPEND(CL_TEMPLATES -i\  ${TEMPLATES})
PREP_GENERATED_SOURCE_FILES(CL_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR} ${TEMPLATES})
string (REPLACE ";" " " CL_SRCS "${SRCS}")

# Final workaround: do this (and cry)
FILE(GENERATE OUTPUT cxx_reflection_$<CONFIG>.cmake CONTENT "execute_process(COMMAND ${CXX_REFLECTION_TOOL}\
 ${CL_SRCS} ${CL_TEMPLATES}\
 --output-jsondb ${CL_OUTPUT_JSON}\
 -- -c -std=c++1z -x c++ ${INCDIRS2} OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_FILE blabla.txt) ")

add_custom_command(
    OUTPUT ${CL_OUTPUTS}
    DEPENDS ${CXX_REFLECTION_TOOL}
    COMMAND ${CMAKE_COMMAND} -P cxx_reflection_$<CONFIG>.cmake )
target_sources(${TARGET} PUBLIC ${CL_OUTPUTS})
endfunction()

#====================================================================
#====================================================================
# Executable target
#====================================================================
#====================================================================
add_executable(pipelines 
	${APP_SOURCES} 
	${APP_UI_SOURCES})

reflection_gen(pipelines ${GEN_SOURCES})

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
