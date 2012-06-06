cmake_minimum_required(VERSION 2.8)

project (ascari)

add_subdirectory(agent)
add_subdirectory(simulator)

add_subdirectory(shared)

include_directories(shared)
include_directories(shared/logog/include)

add_definitions(-Wall -pedantic)


add_subdirectory(viewer)


add_subdirectory(testing)

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
