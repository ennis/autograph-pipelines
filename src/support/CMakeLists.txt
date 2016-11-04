cmake_minimum_required(VERSION 3.2)
add_library(autograph_support STATIC ProjectRoot.cpp Failwith.cpp )
target_include_directories(autograph_support PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(autograph_support PUBLIC cppformat optional variant)


if(UNIX)
	target_link_libraries(autograph_support PUBLIC -lstdc++fs)
endif()
