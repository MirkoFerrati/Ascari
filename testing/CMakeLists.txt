


add_executable(test testrandom.hpp main.cpp ../agent/controller/controller.cpp ../agent/decoder/decoder.cpp 
../shared/dynamic.cpp ../agent/encoder/encoderDet.cpp ../agent/automaton/automatonFSM.cpp
	)

add_executable(test_udp_agent fake_agent_for_udp_tester.cpp ../agent/communication/udp_agent_communicator.cpp)
 find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(test ${Boost_LIBRARIES} pthread)
target_link_libraries(test_udp_agent ${Boost_LIBRARIES} pthread)
target_link_libraries(test ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(test_udp_agent ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
include_directories(../shared/parserYAML/include)

#list( APPEND CMAKE_CXX_FLAGS "-std=c++0x -pedantic ${CMAKE_CXX_FLAGS}")

include_directories(../shared/boost/geometry)
include_directories(../shared/boost/geometry/*)