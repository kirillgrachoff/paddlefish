cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(paddlefish)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(paddlefish)
add_subdirectory(test)
