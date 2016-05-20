# autograph
add_subdirectory(autograph EXCLUDE_FROM_ALL)
target_include_directories(autograph INTERFACE autograph/src/)

# cppformat
add_subdirectory(cppformat EXCLUDE_FROM_ALL)

# mcpp
add_subdirectory(mcpp EXCLUDE_FROM_ALL)
target_include_directories(mcpp INTERFACE mcpp/)


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
file(GLOB NANOVG_SOURCES nanovg/src/*.c)
add_library(nanovg EXCLUDE_FROM_ALL STATIC ${NANOVG_SOURCES})
target_include_directories(nanovg INTERFACE nanovg/src)
target_link_libraries(nanovg PRIVATE stb)
