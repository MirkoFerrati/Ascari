


add_executable(test main.cpp ../agent/controller/controller.cpp ../shared/dynamic.cpp ../agent/encoder/encoderDet.cpp ../agent/topology/topology.cpp ../agent/automaton/automatonFSM.cpp)

 find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(test ${Boost_LIBRARIES} pthread)
#list( APPEND CMAKE_CXX_FLAGS "-std=c++0x -pedantic ${CMAKE_CXX_FLAGS}")
