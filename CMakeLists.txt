cmake_minimum_required(VERSION 2.8)

project (ascari)

add_subdirectory(agent)
add_subdirectory(simulator)
add_subdirectory(gml2lgf)
add_subdirectory(shared)
add_subdirectory(launcher)
include_directories(shared)
include_directories(shared/logog/include)

add_definitions(-std=c++0x -Wall -pedantic -Wmissing-field-initializers -W -Wunused -Wuninitialized -Wformat=2 -Wctor-dtor-privacy -Wnon-virtual-dtor -Wno-char-subscripts -Wwrite-strings -Wno-char-subscripts -Wreturn-type -Wcast-qual -Wcast-align -Wsign-promo -Woverloaded-virtual -fno-strict-aliasing -Wno-unknown-pragmas)


add_subdirectory(viewer)


add_subdirectory(testing)

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
