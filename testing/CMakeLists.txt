


add_executable(test main.cpp ../agent/controller/controller.cpp ../agent/decoder/decoder.cpp ../shared/dynamic.cpp ../agent/encoder/encoderDet.cpp ../agent/automaton/automatonFSM.cpp)

 find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(test ${Boost_LIBRARIES} pthread)
target_link_libraries(test ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)

#list( APPEND CMAKE_CXX_FLAGS "-std=c++0x -pedantic ${CMAKE_CXX_FLAGS}")
