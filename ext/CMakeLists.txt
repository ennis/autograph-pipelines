# 
if (UNIX)
find_package(PkgConfig REQUIRED)
endif()

# zlib (required for skia)
add_subdirectory(zlib)
TARGET_INCLUDE_DIRECTORIES(zlibstatic INTERFACE zlib/)
set_property(TARGET zlibstatic PROPERTY FOLDER "External")

# cppformat
add_subdirectory(cppformat EXCLUDE_FROM_ALL)
set_property(TARGET fmt PROPERTY FOLDER "External")

# mcpp
add_subdirectory(mcpp EXCLUDE_FROM_ALL)
target_include_directories(mcpp INTERFACE mcpp/)
set_property(TARGET mcpp PROPERTY FOLDER "External")

# bustache
#add_subdirectory(bustache EXCLUDE_FROM_ALL)

# assimp
option(BUILD_SHARED_LIBS "..." OFF)
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

# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# GLFW 
option(BUILD_SHARED_LIBS "..." OFF)
option(GLFW_BUILD_EXAMPLES "..." OFF)
option(GLFW_BUILD_TESTS "..." OFF)
option(GLFW_BUILD_DOCS "..." OFF)
option(GLFW_INSTALL "..." OFF)
option(GLFW_DOCUMENT_INTERNALS "..." OFF)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
target_include_directories(glfw INTERFACE glfw/include)

# imgui
file(GLOB IMGUI_SOURCES imgui/*.cpp imgui_glfw3/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC imgui imgui_glfw3)
target_link_libraries(imgui PRIVATE glloadgen glfw glm)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)
target_link_libraries(glm INTERFACE glloadgen)

# glloadgen
add_subdirectory(glloadgen EXCLUDE_FROM_ALL)

# nanovg
#file(GLOB NANOVG_SOURCES nanovg/src/*.c)
#add_library(nanovg EXCLUDE_FROM_ALL STATIC ${NANOVG_SOURCES})
#target_include_directories(nanovg INTERFACE nanovg/src)
#target_link_libraries(nanovg PUBLIC stb)

# variant 
add_library(variant INTERFACE)
target_include_directories(variant INTERFACE variant/include)

# json
add_library(json INTERFACE)
target_include_directories(json INTERFACE json/src)

# range-v3
ADD_LIBRARY(range_v3 INTERFACE) 
TARGET_INCLUDE_DIRECTORIES(range_v3 INTERFACE range-v3/include)

# hana
add_library(hana INTERFACE)
TARGET_INCLUDE_DIRECTORIES(hana INTERFACE hana/include)

# optional
add_library(optional INTERFACE)
TARGET_INCLUDE_DIRECTORIES(optional INTERFACE Optional)

# GSL
add_library(gsl INTERFACE)
TARGET_INCLUDE_DIRECTORIES(gsl INTERFACE GSL)

# msgpack
#add_library(msgpack INTERFACE)
#TARGET_INCLUDE_DIRECTORIES(msgpack INTERFACE msgpack-c/include)

# Skia
#add_subdirectory(skia/cmake)

# mono
#add_library(mono INTERFACE)
#target_include_directories(mono INTERFACE ${MONO_INCLUDE_DIR})
#target_link_libraries(mono INTERFACE ${MONO_LIBRARY})

# luajit-bin
add_library(luajit_bin INTERFACE)
find_package(LuaJIT)
if (LUAJIT_FOUND)
target_include_directories(luajit_bin INTERFACE ${LUAJIT_INCLUDE_DIR})
target_link_libraries(luajit_bin INTERFACE ${LUAJIT_LIBRARY})
message("LuaJIT include: ${LUAJIT_INCLUDE_DIR}")
message("LuaJIT lib: ${LUAJIT_LIBRARY}")
else() # Use bundled binaries for windows
target_include_directories(luajit_bin INTERFACE luajit_bin/include)
target_link_libraries(luajit_bin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/luajit_bin/lib/lua51.lib)
endif()

# lua52compat
#add_library(lua52compat STATIC lua52compat/compat-5.2.c)
#target_link_libraries(lua52compat luajit_bin)

# binary target for skia 
add_library(skia-bin INTERFACE)
target_include_directories(skia-bin INTERFACE 
	skia-bin/include/android
	skia-bin/include/c
	skia-bin/include/codec
	skia-bin/include/core
	skia-bin/include/effects
	skia-bin/include/gpu
	skia-bin/include/gpu/effects
	skia-bin/include/gpu/gl
	skia-bin/include/images
	skia-bin/include/pathops
	skia-bin/include/ports
	skia-bin/include/utils
	skia-bin/include/utils/mac
	skia-bin/include)
# TODO do this only for windows
if (WIN32)
target_link_libraries(skia-bin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/skia-bin/skia.lib)
elseif(UNIX)
find_package(Freetype REQUIRED)
target_link_libraries(skia-bin INTERFACE 
	${CMAKE_CURRENT_SOURCE_DIR}/skia-bin/libskialua.a
	${CMAKE_CURRENT_SOURCE_DIR}/skia-bin/libskia.a 
	${FREETYPE_LIBRARIES}
	-lfontconfig
	)
endif()

# lua
# on windows, compile it from source
if (WIN32)
add_subdirectory(lua-5.3.3 EXCLUDE_FROM_ALL)
# on linux, use find_package
elseif(UNIX)
find_package(Lua 5.3 REQUIRED)
if (LUA_FOUND)
add_library(lua53 INTERFACE)
target_include_directories(lua53 INTERFACE ${LUA_INCLUDE_DIR})
target_link_libraries(lua53 INTERFACE ${LUA_LIBRARIES})
endif()
endif()

#sol2
add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE sol2)
target_link_libraries(sol2 INTERFACE luajit_bin optional)
