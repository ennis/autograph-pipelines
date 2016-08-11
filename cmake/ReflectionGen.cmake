#====================================================================
#====================================================================
# Reflection parser rules
#====================================================================
#====================================================================
SET(CXX_REFLECT_TOOL "${CMAKE_SOURCE_DIR}/tools/cxx-reflect.exe" CACHE FILEPATH "Path to the cxx-reflect executable")

# TODO cleanup
# Don't forget the quotes: https://cmake.org/pipermail/cmake/2015-April/060355.html
# Otherwise the space before -I will be interpreted as an argument separator and will split the generator expression in two!
# Totally intuitive behavior! Bravo CMake developers!
# But wait, there's more! See http://cmake.3232098.n2.nabble.com/Tricky-problem-with-variable-whitespace-quotes-and-shell-td3397636.html
# Fortunately there is a function separate_arguments that you can use!
# Oh no! It doesn't work with generator expressions!
# Ce genre de 'subtilité' est PARTOUT dans CMake. Pas moyen d'avoir un truc qui marche sans avoir à 
# aller chercher un workaround dans stackoverflow (dans le meilleur des cas) ou tomber sur un bug report.
# Ce n'est pas ACCEPTABLE pour un logiciel de cette taille et avec autant d'utilisateurs. 
# Des fois je me demande si les développeurs de CMake en ont quelque chose à foutre.

FUNCTION(reflection_gen TARGET SOURCE TEMPLATES)
# Workaround: custom addprefix function
# GNU make has it. But not CMake, even if it's otherwise a clusterfuck of fragile and useless features.
# Are they trying to make it intentionally difficult?
# BTW It doesn't work with generator expressions, of course.
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

#PREPEND(INCDIRS2 -I ${INCDIRS}) 
#set(MERGED_JSON_DB "meta.gen.json")
SET(INCDIRS2 -I$<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>,\ -I>)
PREP_GENERATED_SOURCE_FILES(CL_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR} ${TEMPLATES})
set(JSON_FILES "")
#FOREACH(SRC ${SOURCE})
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE})
get_filename_component(SRC_FILENAME ${SRC} NAME)
set(SCRIPT "cxx_reflection_$<CONFIG>_${SRC_FILENAME}_${TARGET}.cmake")
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT} CONTENT "execute_process(COMMAND ${CXX_REFLECT_TOOL}\
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
string(REGEX REPLACE "([^;]+)" "${CMAKE_CURRENT_SOURCE_DIR}/\\1" CL_TEMPLATES "${TEMPLATES}")
add_custom_command(
    OUTPUT ${CL_OUTPUTS}
    DEPENDS ${CXX_REFLECT_TOOL} ${JSON_FILES} ${CL_TEMPLATES}
    COMMAND ${CXX_REFLECT_TOOL} --action=render ${CL_TEMPLATES} --jsondb ${JSON_FILES} -- -D__REFLECTION_PARSER__ -c
	VERBATIM )
target_sources(${TARGET} PUBLIC ${CL_OUTPUTS})
endfunction()
