cmake_minimum_required(VERSION 3.21)
project(code)

set(CMAKE_CXX_STANDARD 17)

add_executable(code src/parser.cpp src/bitset32.cpp src/main.cpp)
include_directories("include")