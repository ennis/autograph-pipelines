cmake_minimum_required(VERSION 3.2)
add_library(autograph_support SHARED 
	ProjectRoot.cpp 
	Debug.cpp 
	SmallVector.cpp
	FileDialog.cpp
	IDTable.cpp)
target_include_directories(autograph_support PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(autograph_support 
	PRIVATE
	nativefiledialog 
	PUBLIC 
	fmt
	optional
	variant
	gsl
	glm)
target_compile_definitions(autograph_support PRIVATE -DAG_SUPPORT_DLL -DAG_BUILD_SUPPORT_DLL)
target_compile_definitions(autograph_support INTERFACE -DAG_SUPPORT_DLL)
set_property(TARGET autograph_support PROPERTY FOLDER "Engine")

if (WIN32)
target_compile_definitions(autograph_support PUBLIC -DAG_WINDOWS)
target_compile_options(autograph_support PUBLIC /wd4251)
elseif(UNIX)
target_compile_definitions(autograph_support PUBLIC -DAG_UNIX)
endif()

if(UNIX)
	target_link_libraries(autograph_support PUBLIC -lstdc++fs)
endif()
