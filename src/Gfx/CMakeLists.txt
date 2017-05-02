set(NAME autographGfx)
file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.hpp)

autograph_add_module(SHARED 
	NAME autographGfx
	INCDIR autograph/Gfx 
	SHORTNAME AG_GFX 
	SOURCES ${LIB_SOURCES})
target_link_libraries(autographGfx 
	PUBLIC
		autographCore 
		OpenGL
	PRIVATE
		glm 
		glfw 
		fmt)

