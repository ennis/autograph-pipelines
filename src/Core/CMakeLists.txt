file(GLOB SRC *.cpp Support/*.cpp)

autograph_add_module(
	SHARED 
	NAME autographCore
	INCDIR autograph/Core 
	SHORTNAME AG_CORE
	SOURCES ${SRC})

# force C++17 on MSVC
target_compile_options(autographCore PUBLIC "$<$<PLATFORM_ID:Windows>:/std:c++latest>")

target_link_libraries(autographCore 
	PUBLIC 
		fmt glm gsl optional variant nativefiledialog
		$<$<PLATFORM_ID:Linux>:-lstdc++fs>
	)
