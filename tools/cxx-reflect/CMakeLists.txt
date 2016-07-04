message(STATUS "Looking for LLVM in ${LLVM_DIR}")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
set(CMAKE_CXX_STANDARD 14)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${CLANG_DEFINITIONS})

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.hpp)

add_executable(cxx-reflect ${SOURCES})

set(CLANG_LIBRARIES 
	clangBasic 
	clangLex 
	clangParse 
	clangAST 
	clangDynamicASTMatchers 
	clangASTMatchers 
	clangSema 
	clangAnalysis 
	clangEdit 
	clangRewrite 
	clangDriver 
	clangRewriteFrontend 
	clangFrontend 
	clangFrontendTool 
	clangToolingCore 
	clangTooling 
	clangIndex)

# Link against LLVM libraries
target_link_libraries(cxx-reflect ${LLVM_LIBRARIES} ${CLANG_LIBRARIES} bustache cppformat)