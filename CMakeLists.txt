cmake_minimum_required(VERSION 3.5)
project(AStar)
FILE(GLOB path_headers *.h)
FILE(GLOB path_sources *.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -static -std=gnu++11")

set(SOURCE_FILES ${path_sources} ${path_headers})
add_executable(vastproj ${SOURCE_FILES})

source_group("source" FILES  ${path_sources})
source_group("header" FILES  ${path_headers})
