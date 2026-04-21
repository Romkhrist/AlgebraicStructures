cmake_minimum_required(VERSION 3.5)

project(AlgebraicCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_executable(main)

target_sources(main
	PUBLIC
		FILE_SET CXX_MODULES FILES
			src/algebraic.ixx
	PRIVATE
		src/main.cpp
)