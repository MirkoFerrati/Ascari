


add_executable(test main.cpp)

include_directories(agent shared simulator)

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
