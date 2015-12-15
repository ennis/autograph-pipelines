# Here we (re-)define cmake targets for the libraries
# Note: we could use the provided CMakeLists.txt, but we cannot set 
# any goddamned project options with 'add_subdirectory'. 
# For example, with cppformat, 'add_subdirectory' will add all tests and examples
# to the build (and we cannot even put them in a separate group for visual studio!)

# cppformat
file(GLOB CPPFORMAT_SOURCES cppformat/*.cc)
file(GLOB CPPFORMAT_HEADERS cppformat/*.h)
add_library(cppformat ${CPPFORMAT_SOURCES} ${CPPFORMAT_HEADERS})
set_property(TARGET cppformat PROPERTY FOLDER "External")
target_include_directories(cppformat INTERFACE cppformat)

# glbinding
add_subdirectory(glbinding)
set_property(TARGET glbinding PROPERTY FOLDER "External")
target_include_directories(glbinding INTERFACE glbinding/source/glbinding/include)
if(MSVC)
	# disable warning 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
	target_compile_options(glbinding INTERFACE /wd4251)
endif(MSVC)
