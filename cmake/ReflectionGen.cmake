#====================================================================
#====================================================================
# Reflection parser rules
#====================================================================
#====================================================================
set(CXX_REFLECT_TOOL "${CMAKE_SOURCE_DIR}/tools/cxx-reflect.exe" CACHE FILEPATH "Path to the cxx-reflect executable")
set(TEMPLATE_TOOL "${CMAKE_SOURCE_DIR}/tools/tpl_compile.lua" CACHE FILEPATH "Path to the template renderer script")
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/tools/" CACHE FILEPATH "Path to the tools directory")

# use cases:
# Generate JSON database
# Generate C++ source file from JSON 
# Generate external source file from JSON (C#)

function(reflection_generate_database TARGET SOURCE)
  set(INCDIRS2 -I$<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>,\ -I>)
  get_filename_component(SRC_FILENAME ${SOURCE} NAME)
  string(SHA1 SRCNAMEHASH ${SOURCE}_${TARGET})
  set(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SRCNAMEHASH}.json)
  set(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/cxx_reflection_$<CONFIG>_${SRCNAMEHASH}.cmake)
  file(GENERATE OUTPUT ${SCRIPT} CONTENT "execute_process(COMMAND ${CXX_REFLECT_TOOL}\
    ${SOURCE} -o ${OUTPUT} -- -D__REFLECTION_PARSER__ -v -c -std=c++1z -x c++ ${INCDIRS2} OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE ERROR_FILE blabla.txt) ")
  add_custom_command(
	OUTPUT ${OUTPUT}
    DEPENDS ${CXX_REFLECTION_TOOL} ${SOURCE}
	IMPLICIT_DEPENDS CXX ${SOURCE}
    COMMAND ${CMAKE_COMMAND} -P ${SCRIPT})
  set_target_properties(${TARGET} PROPERTIES REFLECTION_DATABASE_PATH ${OUTPUT})
endfunction()


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


# Generate C++ source files from the reflection data of targets and add them to a target
function(reflection_render_template TARGET REFLECTION_TARGET SOURCE_TEMPLATES)
get_target_property(REFLECTION_DB ${REFLECTION_TARGET} REFLECTION_DATABASE_PATH)
PREP_GENERATED_SOURCE_FILES(OUTPUTS ${CMAKE_CURRENT_BINARY_DIR} ${SOURCE_TEMPLATES})
add_custom_command(
    OUTPUT ${OUTPUTS}
    DEPENDS ${REFLECTION_DB} ${SOURCE_TEMPLATES}
	WORKING_DIRECTORY ${TOOLS_DIR}
    COMMAND luajit ${TEMPLATE_TOOL} ${REFLECTION_DB} ${OUTPUTS} ${SOURCE_TEMPLATES}
	VERBATIM )
target_sources(${TARGET} PUBLIC ${OUTPUTS})
endfunction()

#function()