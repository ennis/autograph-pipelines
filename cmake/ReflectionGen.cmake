#====================================================================
#====================================================================
# Reflection parser rules
#====================================================================
#====================================================================
set(CXX_REFLECT_TOOL "${CMAKE_SOURCE_DIR}/tools/cxx-reflect.exe" CACHE FILEPATH "Path to the cxx-reflect executable")

function(add_reflection_target TARGET SOURCE)
  set(INCDIRS2 -I$<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>,\ -I>)
  set(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILENAME}.json)
  get_filename_component(SRC_FILENAME ${SOURCE} NAME)
  set(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/cxx_reflection_$<CONFIG>_${SRC_FILENAME}_${TARGET}.cmake)

  file(GENERATE OUTPUT ${SCRIPT} CONTENT "execute_process(COMMAND ${CXX_REFLECT_TOOL}\
    ${SOURCE} -o ${OUTPUT} -- -D__REFLECTION_PARSER__ -v -c -std=c++1z -x c++ ${INCDIRS2} OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE ERROR_FILE blabla.txt) ")
  add_custom_target(${TARGET}_reflection
    DEPENDS ${CXX_REFLECTION_TOOL} ${SOURCE}
    COMMAND ${CMAKE_COMMAND} -P ${SCRIPT})
endfunction()


# use cases:
# Generate JSON database
# Generate C++ source file from JSON 
# Generate external source file from JSON (C#)


#function()