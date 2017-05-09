# Macro to set the shared library flag for subprojects
macro(OVERRIDE_BUILD_SHARED arg)
set(BUILD_SHARED_LIBS ${arg} CACHE BOOL "..." FORCE)
endmacro()

#------------------------------------------
# OpenGL
find_package(OpenGL REQUIRED)
add_library(OpenGL INTERFACE)
target_include_directories(OpenGL INTERFACE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(OpenGL INTERFACE ${OPENGL_LIBRARIES})

#------------------------------------------
# Boost
#find_package(Boost REQUIRED COMPONENTS thread filesystem system context wave)

#--------------------------------------------------------------------
# PkgConfig
if (UNIX)
find_package(PkgConfig REQUIRED)
endif()


#--------------------------------------------------------------------
# zlib (required for skia)
#[[find_package(ZLIB)
if(ZLIB_FOUND)
message(STATUS "Using system provided zlib")
message(STATUS " - include dirs: ${ZLIB_INCLUDE_DIRS}")
message(STATUS " - libraries: ${ZLIB_LIBRARIES}")
add_library(zlibstatic INTERFACE)
target_include_directories(zlibstatic INTERFACE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(zlibstatic INTERFACE ${ZLIB_LIBRARIES})
else()
message(STATUS "Building own zlib")
add_subdirectory(zlib)
TARGET_INCLUDE_DIRECTORIES(zlibstatic INTERFACE zlib/)
set_property(TARGET zlibstatic PROPERTY FOLDER "External")
endif()]]

#--------------------------------------------------------------------
# cppformat
OVERRIDE_BUILD_SHARED(OFF)
add_subdirectory(cppformat EXCLUDE_FROM_ALL)
set_property(TARGET fmt PROPERTY FOLDER "External")


#--------------------------------------------------------------------
# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)
#target_link_libraries(glm INTERFACE glloadgen)


#--------------------------------------------------------------------
# variant 
add_library(variant INTERFACE)
target_include_directories(variant INTERFACE variant/include)


#--------------------------------------------------------------------
# json
add_library(json INTERFACE)
target_include_directories(json INTERFACE json/src)


#--------------------------------------------------------------------
# range-v3
#ADD_LIBRARY(range_v3 INTERFACE) 
#TARGET_INCLUDE_DIRECTORIES(range_v3 INTERFACE range-v3/include)


#--------------------------------------------------------------------
# hana
add_library(hana INTERFACE)
TARGET_INCLUDE_DIRECTORIES(hana INTERFACE hana/include)


#--------------------------------------------------------------------
# optional
add_library(optional INTERFACE)
TARGET_INCLUDE_DIRECTORIES(optional INTERFACE Optional)


#--------------------------------------------------------------------
# GSL
add_library(gsl INTERFACE)
TARGET_INCLUDE_DIRECTORIES(gsl INTERFACE GSL)


#--------------------------------------------------------------------
# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)


#--------------------------------------------------------------------
# mcpp
add_subdirectory(mcpp EXCLUDE_FROM_ALL)
target_include_directories(mcpp INTERFACE mcpp/)
set_property(TARGET mcpp PROPERTY FOLDER "External")

# bustache
#add_subdirectory(bustache EXCLUDE_FROM_ALL)


#--------------------------------------------------------------------
# assimp
find_package(Assimp)
if (Assimp_FOUND)
# use system package
add_library(assimp SHARED IMPORTED GLOBAL)
message( STATUS "Using system-provided Assimp library." )
message(STATUS " - include dirs: ${ASSIMP_INCLUDE_DIRS}")
message(STATUS " - libraries: ${ASSIMP_LIBRARIES}")
set_property(TARGET assimp PROPERTY IMPORTED_LOCATION "${ASSIMP_LIBRARIES}")
set_property(TARGET assimp PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${ASSIMP_INCLUDE_DIRS}")
else()
message( STATUS "Building Assimp library." )
# compile bundled
OVERRIDE_BUILD_SHARED(ON)
option(ASSIMP_NO_EXPORT "..." ON)
option(ASSIMP_BUILD_ASSIMP_TOOLS "..." OFF)
option(ASSIMP_BUILD_SAMPLES "..." OFF)
option(ASSIMP_BUILD_TESTS "..." OFF)
option(ASSIMP_BUILD_OBJ_IMPORTER "..." ON)
option(ASSIMP_BUILD_3DS_IMPORTER "..." ON)
option(ASSIMP_BUILD_FBX_IMPORTER "..." ON)
option(ASSIMP_BUILD_IFC_IMPORTER "..." OFF)
option(ASSIMP_BUILD_OGRE_IMPORTER "..." OFF)
option(ASSIMP_BUILD_Q3BSP_IMPORTER "..." OFF)
option(ASSIMP_BUILD_Q3D_IMPORTER "..." OFF)
option(ASSIMP_BUILD_RAW_IMPORTER "..." OFF)
option(ASSIMP_BUILD_OFF_IMPORTER "..." OFF)
option(ASSIMP_BUILD_NFF_IMPORTER "..." OFF)
option(ASSIMP_BUILD_NDO_IMPORTER "..." OFF)
option(ASSIMP_BUILD_MS3D_IMPORTER "..." OFF)
option(ASSIMP_BUILD_MDL_IMPORTER "..." OFF)
option(ASSIMP_BUILD_IRRMESH_IMPORTER "..." OFF)
option(ASSIMP_BUILD_MDC_IMPORTER "..." OFF)
option(ASSIMP_BUILD_MD5_IMPORTER "..." OFF)
option(ASSIMP_BUILD_Q3D_IMPORTER "..." OFF)
option(ASSIMP_BUILD_3D_IMPORTER "..." OFF)
option(ASSIMP_BUILD_AC_IMPORTER "..." OFF)
option(ASSIMP_BUILD_ASE_IMPORTER "..." OFF)
option(ASSIMP_BUILD_ASSBIN_IMPORTER "..." OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS "..." OFF)
option(ASSIMP_BUILD_ASSXML_IMPORTER "..." OFF)
option(ASSIMP_BUILD_B3D_IMPORTER "..." OFF)
option(ASSIMP_BUILD_BLEND_IMPORTER "..." OFF)
option(ASSIMP_BUILD_BVH_IMPORTER "..." OFF)
option(ASSIMP_BUILD_COB_IMPORTER "..." OFF)
option(ASSIMP_BUILD_CSM_IMPORTER "..." OFF)
option(ASSIMP_BUILD_DXF_IMPORTER "..." OFF)
option(ASSIMP_BUILD_GLTF_IMPORTER "..." OFF)
option(ASSIMP_BUILD_HMP_IMPORTER "..." OFF)
option(ASSIMP_BUILD_IFC_IMPORTER "..." OFF)
option(ASSIMP_BUILD_IRR_IMPORTER "..." OFF)
option(ASSIMP_BUILD_LWO_IMPORTER "..." OFF)
option(ASSIMP_BUILD_LWS_IMPORTER "..." OFF)
option(ASSIMP_BUILD_MD2_IMPORTER "..." OFF)
option(ASSIMP_BUILD_MD3_IMPORTER "..." OFF)
option(ASSIMP_BUILD_OPENGEX_IMPORTER "..." OFF)
option(ASSIMP_BUILD_XGL_IMPORTER "..." OFF)
option(ASSIMP_BUILD_SMD_IMPORTER "..." OFF)
option(ASSIMP_BUILD_SIB_IMPORTER "..." OFF)
option(ASSIMP_BUILD_TERRAGEN_IMPORTER "..." OFF)
option(ASSIMP_BUILD_X_IMPORTER "..." OFF)
add_subdirectory(assimp EXCLUDE_FROM_ALL)
target_include_directories(assimp INTERFACE assimp/include)
set_property(TARGET assimp PROPERTY FOLDER "External")
endif()

#--------------------------------------------------------------------
# GLFW 
OVERRIDE_BUILD_SHARED(ON)
option(GLFW_BUILD_EXAMPLES "..." OFF)
option(GLFW_BUILD_TESTS "..." OFF)
option(GLFW_BUILD_DOCS "..." OFF)
option(GLFW_INSTALL "..." OFF)
option(GLFW_DOCUMENT_INTERNALS "..." OFF)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
target_include_directories(glfw INTERFACE glfw/include)
set_property(TARGET glfw PROPERTY FOLDER "External")


#--------------------------------------------------------------------
# OpenEXR
OVERRIDE_BUILD_SHARED(OFF)
if (UNIX)
pkg_check_modules(OpenEXR OpenEXR)
endif()
if (OpenEXR_FOUND) 
	# Use system package
	message("Using OpenEXR from system package")
	add_library(OpenEXR INTERFACE)
	target_include_directories(OpenEXR INTERFACE ${OpenEXR_INCLUDE_DIRS})
	target_link_libraries(OpenEXR INTERFACE ${OpenEXR_LIBRARIES})
elseif (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/openexr_bin AND WIN32)
	# Use binaries
	message("Using OpenEXR binaries in ext/openexr_bin")
	add_library(OpenEXR INTERFACE)
	target_include_directories(OpenEXR INTERFACE openexr_bin/include)
	target_link_libraries(OpenEXR INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}/openexr_bin/lib/IlmImf.lib
		${CMAKE_CURRENT_SOURCE_DIR}/openexr_bin/lib/Iex.lib
		${CMAKE_CURRENT_SOURCE_DIR}/openexr_bin/lib/Half.lib
		${CMAKE_CURRENT_SOURCE_DIR}/openexr_bin/lib/IlmThread.lib
		${CMAKE_CURRENT_SOURCE_DIR}/openexr_bin/lib/Imath.lib)
else()
	# Compile from source
	message("Compiling OpenEXR from source")
	# hack to find half.h and friends
	OVERRIDE_BUILD_SHARED(ON)
	option(BUILD_SHARED_LIBS    "..." OFF)
	option(USE_ZLIB_WINAPI      "..." OFF)
	option(NAMESPACE_VERSIONING "..." OFF)
	add_subdirectory(openexr/IlmBase EXCLUDE_FROM_ALL)
	get_property(ILMBASE_INCLUDE_DIRS DIRECTORY openexr/IlmBase PROPERTY INCLUDE_DIRECTORIES)
	include_directories(${ILMBASE_INCLUDE_DIRS})
	add_subdirectory(openexr/OpenEXR EXCLUDE_FROM_ALL)
	get_property(ILMIMF_INCLUDE_DIRS DIRECTORY openexr/OpenEXR PROPERTY INCLUDE_DIRECTORIES)
	add_library(OpenEXR INTERFACE)
	target_include_directories(OpenEXR INTERFACE ${ILMIMF_INCLUDE_DIRS})
	target_link_libraries(OpenEXR INTERFACE IlmImf)
	#set_property(TARGET OpenEXR PROPERTY FOLDER "External/OpenEXR")
endif()

#--------------------------------------------------------------------
# OpenImageIO 
find_package(OpenImageIO)
if (${OpenImageIO_FOUND})
	message("Found OpenImageIO: ${OPENIMAGEIO_INCLUDE_DIRS}")
	add_library(OpenImageIO INTERFACE)
	target_include_directories(OpenImageIO INTERFACE ${OPENIMAGEIO_INCLUDE_DIRS})
	target_link_libraries(OpenImageIO INTERFACE ${OPENIMAGEIO_LIBRARIES})
else()
# use bundled version
	add_library(OpenImageIO SHARED IMPORTED GLOBAL)
	message( STATUS "Using OpenImageIO binaries." )
	set_property(TARGET OpenImageIO PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/OpenImageIO.dll")
	set_property(TARGET OpenImageIO PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/lib/OpenImageIO.lib")
	set_property(TARGET OpenImageIO PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/include")
	message( STATUS "Copying OpenImageIO shared libraries." )
	set(OIIO_ExternalSharedLibraries 
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/ptex.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/Half.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/Iex-2_2.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/IexMath-2_2.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/IlmImf-2_2.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/IlmImfUtil-2_2.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/IlmThread-2_2.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/Imath-2_2.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/boost_chrono-vc140-mt-1_63.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/boost_date_time-vc140-mt-1_63.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/boost_filesystem-vc140-mt-1_63.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/boost_system-vc140-mt-1_63.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/boost_thread-vc140-mt-1_63.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/boost_timer-vc140-mt-1_63.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/jpeg62.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/libpng16.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/tiff.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/tiffxx.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/turbojpeg.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/OpenImageIO_bin/bin/OpenImageIO.dll"
	)
	file(COPY ${OIIO_ExternalSharedLibraries} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" NO_SOURCE_PERMISSIONS)
endif()

#--------------------------------------------------------------------
# nanovg
#file(GLOB NANOVG_SOURCES nanovg/src/*.c)
#add_library(nanovg EXCLUDE_FROM_ALL STATIC ${NANOVG_SOURCES})
#target_include_directories(nanovg INTERFACE nanovg/src)
#target_link_libraries(nanovg PUBLIC stb)

# msgpack
#add_library(msgpack INTERFACE)
#TARGET_INCLUDE_DIRECTORIES(msgpack INTERFACE msgpack-c/include)

# Skia
#add_subdirectory(skia/cmake)

# mono
#add_library(mono INTERFACE)
#target_include_directories(mono INTERFACE ${MONO_INCLUDE_DIR})
#target_link_libraries(mono INTERFACE ${MONO_LIBRARY})

#--------------------------------------------------------------------
# luajit-bin
add_library(luajit_bin INTERFACE)
find_package(LuaJIT)
if (LUAJIT_FOUND)
target_include_directories(luajit_bin INTERFACE ${LUAJIT_INCLUDE_DIR})
target_link_libraries(luajit_bin INTERFACE ${LUAJIT_LIBRARY})
message("LuaJIT include: ${LUAJIT_INCLUDE_DIR}")
message("LuaJIT lib: ${LUAJIT_LIBRARY}")
else() # Use bundled binaries for windows
	if (WIN32)
		message("Trying LuaJIT binaries in ext/luajit_bin")
		target_include_directories(luajit_bin INTERFACE luajit_bin/include)
		target_link_libraries(luajit_bin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/luajit_bin/lib/lua51.lib)
	endif()
endif()

# lua52compat
#add_library(lua52compat STATIC lua52compat/compat-5.2.c)
#target_link_libraries(lua52compat luajit_bin)

# binary target for skia 
#add_library(skia-bin INTERFACE)
#target_include_directories(skia-bin INTERFACE 
#	skia-bin/include/android
#	skia-bin/include/c
#	skia-bin/include/codec
#	skia-bin/include/core
#	skia-bin/include/effects
#	skia-bin/include/gpu
#	skia-bin/include/gpu/effects
#	skia-bin/include/gpu/gl
#	skia-bin/include/images
#	skia-bin/include/pathops
#	skia-bin/include/ports
#	skia-bin/include/utils
#	skia-bin/include/utils/mac
#	skia-bin/include)
# TODO do this only for windows
#if (WIN32)
#target_link_libraries(skia-bin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/skia-bin/skia.lib)
#elseif(UNIX)
#find_package(Freetype REQUIRED)
#target_link_libraries(skia-bin INTERFACE 
#	${CMAKE_CURRENT_SOURCE_DIR}/skia-bin/libskialua.a
#	${CMAKE_CURRENT_SOURCE_DIR}/skia-bin/libskia.a 
#	${FREETYPE_LIBRARIES}
#	-lfontconfig
#	)
#endif()

# lua
# on windows, compile it from source
#if (WIN32)
#add_subdirectory(lua-5.3.3 EXCLUDE_FROM_ALL)
# on linux, use find_package
#elseif(UNIX)
#find_package(Lua 5.3 REQUIRED)
#if (LUA_FOUND)
#add_library(lua53 INTERFACE)
#target_include_directories(lua53 INTERFACE ${LUA_INCLUDE_DIR})
#target_link_libraries(lua53 INTERFACE ${LUA_LIBRARIES})
#endif()
#endif()

#--------------------------------------------------------------------
#sol2
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE sol2)
target_link_libraries(sol2 INTERFACE luajit_bin optional)

#--------------------------------------------------------------------
# nativefiledialog
set(NFD_SOURCES_COMMON nativefiledialog/src/nfd_common.c)
if (WIN32)
set(NFD_SOURCES_ARCH nativefiledialog/src/nfd_win.cpp)
elseif(UNIX)
set(NFD_SOURCES_ARCH nativefiledialog/src/nfd_gtk.c)
endif()
add_library(nativefiledialog STATIC ${NFD_SOURCES_COMMON} ${NFD_SOURCES_ARCH})
target_include_directories(nativefiledialog PUBLIC nativefiledialog/src/include)
if(UNIX)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
target_include_directories(nativefiledialog PUBLIC ${GTK3_INCLUDE_DIRS})
#target_compile_definitions(nativefiledialog PUBLIC ${GTK3_CFLAGS_OTHER})
target_link_libraries(nativefiledialog PUBLIC ${GTK3_LIBRARIES})
endif()
set_property(TARGET nativefiledialog PROPERTY FOLDER "External")

