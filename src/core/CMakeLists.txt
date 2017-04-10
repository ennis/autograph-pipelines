file(GLOB SRC *.cpp)

############## Config.h ##############
add_library(autograph_core SHARED ${SRC})
target_include_directories(autograph_core PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(autograph_core PUBLIC fmt glm)
target_compile_definitions(autograph_core PRIVATE -DAG_CORE_DLL -DAG_BUILD_CORE_DLL)
target_compile_definitions(autograph_core INTERFACE -DAG_CORE_DLL)
set_property(TARGET autograph_core PROPERTY FOLDER "Engine")
if (WIN32)
target_compile_options(autograph_core PUBLIC /wd4251)
endif()


