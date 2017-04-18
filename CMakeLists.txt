cmake_minimum_required(VERSION 3.2)

project(autograph C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-DNOMINMAX -DGLM_FORCE_RADIANS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(AG_PUBLIC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(AG_VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(AG_VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(AG_VERSION_PATCH   1   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include("cmake/ReflectionGen.cmake")

#####################################
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


######################################
############## Hack no1 ##############
if(UNIX)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++fs")
        #include_directories("/usr/include/c++/v1/")
endif()

##############################################
############## Force clang modules ###########
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules -fmodules-cache-path=${CMAKE_SOURCE_DIR}/cache")

###########################################
############## Config header ##############
execute_process(
     COMMAND git rev-parse head
     OUTPUT_VARIABLE GIT_VERSION
     OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 
set(PROJECT_DIR_NAME "autograph")

configure_file(
    "${PROJECT_SOURCE_DIR}/include/autograph/Config.h.in"
    "${PROJECT_BINARY_DIR}/include/autograph/Config.h")

include_directories("${PROJECT_BINARY_DIR}/include")

##############################################
############## External modules ##############
find_package(OpenGL REQUIRED)
#find_package(Mono REQUIRED)
#link_directories(${MONO_LIBRARY_DIRS})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread filesystem system context REQUIRED)

add_subdirectory(ext EXCLUDE_FROM_ALL)

##########################################
############## Core modules ##############
add_subdirectory(src_core/math)
add_subdirectory(src_core/support)
add_subdirectory(src_core/gl)
add_subdirectory(src_core/engine)
add_subdirectory(src_core/editor)
add_subdirectory(src_core/reflection_gen)
add_subdirectory(src_core/editor_shell)
add_subdirectory(src_core/main)

####################################################
############## Hot-reloadable modules ##############
function(autograph_add_module NAME SOURCES)
    add_library(${NAME} SHARED ${SOURCES})
    target_compile_definitions(${NAME} PRIVATE -DAG_MODULE -D${NAME}_DLL -DBUILD_${NAME}_DLL)
    target_link_libraries(${NAME} PUBLIC autograph_engine)
    target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
    target_include_directories(${NAME} PUBLIC $<TARGET_PROPERTY:autograph_engine,INCLUDE_DIRECTORIES>)
    set(PROJECT_NAME ${NAME})
    configure_file (
        "${PROJECT_SOURCE_DIR}/src_plugins/ModuleConfig.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.Config.h"
    )
    target_include_directories(${NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/")
    #reflection_generate_database(${NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Module.h")
    #reflection_render_template(${NAME} ${NAME} "${PROJECT_SOURCE_DIR}/src/ModuleMeta.cpp.in")
    set_property(TARGET ${NAME} PROPERTY FOLDER "Plugins")
    MESSAGE("Autograph plugin added: ${NAME}")
endfunction()

##############################################
############## Load all plugins ##############
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

subdirlist(PLUGINS_SUBDIRS ${PROJECT_SOURCE_DIR}/src_plugins)

foreach(pluginSubdirectory ${PLUGINS_SUBDIRS})
  add_subdirectory(src_plugins/${pluginSubdirectory})
endforeach()
