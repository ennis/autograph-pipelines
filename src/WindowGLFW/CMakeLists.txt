set(NAME autographWindowGLFW)
file(GLOB SRC *.cpp)

autograph_add_module(
	STATIC 
	NAME ${NAME} 
	SHORTNAME "WindowGLFW" 
	SOURCES ${SRC})

target_link_libraries(${NAME} PUBLIC autographEngine glfw)
