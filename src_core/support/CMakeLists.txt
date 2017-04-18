cmake_minimum_required(VERSION 3.2)

set(NAME autograph_support)
add_library(${NAME} SHARED 
	ProjectRoot.cpp 
	Debug.cpp 
	SmallVector.cpp
	FileDialog.cpp
	IDTable.cpp)
target_include_directories(${NAME} PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(${NAME} 
	PRIVATE
	nativefiledialog 
	PUBLIC 
	fmt
	optional
	variant
	gsl
	glm)
target_compile_definitions(${NAME} PRIVATE -DAG_SUPPORT_DLL -DAG_BUILD_SUPPORT_DLL)
target_compile_definitions(${NAME} INTERFACE -DAG_SUPPORT_DLL)
set_property(TARGET ${NAME} PROPERTY FOLDER "Core")

if (WIN32)
target_compile_definitions(${NAME} PUBLIC -DAG_WINDOWS)
target_compile_options(${NAME} PUBLIC /wd4251)
elseif(UNIX)
target_compile_definitions(${NAME} PUBLIC -DAG_UNIX)
endif()

if(UNIX)
	target_link_libraries(${NAME} PUBLIC -lstdc++fs)
endif()
