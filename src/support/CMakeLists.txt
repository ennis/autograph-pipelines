cmake_minimum_required(VERSION 3.2)
add_library(autograph_support STATIC 
	ProjectRoot.cpp 
	Debug.cpp 
	SmallVector.cpp
	FileDialog.cpp
	IDTable.cpp)
target_include_directories(autograph_support PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(autograph_support PUBLIC 
	fmt
	optional
	variant
	gsl
	glm
	nativefiledialog)

if (WIN32)
target_compile_definitions(autograph_support PUBLIC -DAG_WINDOWS)
elseif(UNIX)
target_compile_definitions(autograph_support PUBLIC -DAG_UNIX)
endif()

if(UNIX)
	target_link_libraries(autograph_support PUBLIC -lstdc++fs)
endif()
