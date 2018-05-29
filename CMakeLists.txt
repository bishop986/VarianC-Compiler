cmake_minimum_required( VERSION 2.8 )

project(vcc)

add_definitions(-std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-D _DEBUG_")

set(SOURCE_DIR 
	./main.cc
	./scanner/token.cc
	./scanner/scan.cc
	./parser/analysis.cc
	./parser/syntax_treenode.cc)

add_executable(${PROJECT_NAME} ${SOURCE_DIR})
