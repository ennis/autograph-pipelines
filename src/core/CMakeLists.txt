file(GLOB SRC *.cpp ui/*.cpp)
add_library(autograph_core ${SRC})
target_include_directories(autograph_core PUBLIC ${AG_PUBLIC_INCLUDE_DIR})
target_link_libraries(autograph_core cppformat)
