message(STATUS "Looking for LLVM in ${LLVM_DIR}")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
set(CMAKE_CXX_STANDARD 14)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${CLANG_DEFINITIONS})

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.hpp)

add_executable(cxx-reflect ${SOURCES})

set(CLANG_LIBRARIES 
	clangBasic 
	clangLex 
	clangParse 
	clangAST 
	clangDynamicASTMatchers 
	clangASTMatchers 
	clangSema 
	clangAnalysis 
	clangEdit 
	clangRewrite 
	clangDriver 
	clangRewriteFrontend 
	clangFrontend 
	clangFrontendTool 
	clangToolingCore 
	clangTooling 
	clangIndex)

# Link against LLVM libraries
target_link_libraries(cxx-reflect ${LLVM_LIBRARIES} ${CLANG_LIBRARIES} fmt json)

#====================================================================
#====================================================================
# Reflection parser rules
#====================================================================
#====================================================================

# TODO cleanup
FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar ${prefix}${f})
   ENDFOREACH(f)
   SET(${var} ${listVar} PARENT_SCOPE)
ENDFUNCTION(PREPEND)

FUNCTION(APPEND var postfix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar ${f}${postfix})
   ENDFOREACH(f)
   SET(${var} ${listVar} PARENT_SCOPE)
ENDFUNCTION(APPEND)

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


function(reflection_gen TARGET SOURCE TEMPLATES)
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

set(MERGED_JSON_DB "meta.gen.json")
PREPEND(CL_TEMPLATES -i\  ${TEMPLATES})
PREP_GENERATED_SOURCE_FILES(CL_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR} ${TEMPLATES})

SET(JSON_FILES "")

# XXX actually there should be only one source
#FOREACH(SRC ${SOURCE})
SET(SRC ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE})
GET_FILENAME_COMPONENT(SRC_FILENAME ${SRC} NAME)
set(SCRIPT "cxx_reflection_$<CONFIG>_${SRC_FILENAME}_${TARGET}.cmake")
FILE(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT} CONTENT "execute_process(COMMAND ${CXX_REFLECTION_TOOL}\
 --action=reflect ${SRC} -- -D__REFLECTION_PARSER__ -c -std=c++1z -x c++ ${INCDIRS2} OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE ERROR_FILE blabla.txt) ")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILENAME}.json
    DEPENDS ${CXX_REFLECTION_TOOL} ${SRC}
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT})
list(APPEND JSON_FILES ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILENAME}.json)
#ENDFOREACH()

# merge rule
#add_custom_command(
#    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MERGED_JSON_DB}
#    DEPENDS ${CXX_REFLECTION_TOOL} ${JSON_FILES}
#    COMMAND ${CXX_REFLECTION_TOOL} ${JSON_FILES} --action=merge --output-jsondb ${MERGED_JSON_DB} 
#	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#	VERBATIM )

# template generation rule
add_custom_command(
    OUTPUT ${CL_OUTPUTS}
    DEPENDS ${CXX_REFLECTION_TOOL} ${JSON_FILES} ${TEMPLATES}
    COMMAND ${CXX_REFLECTION_TOOL} --action=render ${TEMPLATES} --jsondb ${JSON_FILES}
	VERBATIM )

target_sources(${TARGET} PUBLIC ${CL_OUTPUTS})
endfunction()
