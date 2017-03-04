cmake_minimum_required(VERSION 3.6)
project(any)

set(CMAKE_CXX_COMPILER g++-7)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O0")

set(SOURCE_FILES main.cpp any.h detail.h)
add_executable(any ${SOURCE_FILES})