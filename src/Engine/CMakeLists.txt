set(NAME autographEngine)
file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.hpp)
file(GLOB IMGUI_SOURCES imgui/*.cpp)

find_package(Boost REQUIRED COMPONENTS wave)

autograph_add_module(
	SHARED 
	NAME autographEngine
	INCDIR autograph/Engine 
	SHORTNAME AG_ENGINE 
	SOURCES ${LIB_SOURCES} ${LIB_HEADERS} ${IMGUI_SOURCES})

target_link_libraries(
		${NAME} 
	PRIVATE
		##### external
		zlibstatic
		luajit_bin
		stb
		fmt 
	PUBLIC
		autographCore # dynamic
		autographGfx # dynamic
		sol2 
		json
		variant
		optional
		glm 
		mcpp
		OpenImageIO
		$<$<PLATFORM_ID:Linux>:${CMAKE_DL_LIBS}>
	)
