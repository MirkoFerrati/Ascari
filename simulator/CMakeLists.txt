

add_executable(simulator ../shared/dynamic.cpp communication/udp_agent_communicator.cpp simulator.cpp main.cpp communication/agent_communicator_abstract.h ../shared/yaml_parser.cpp)

add_subdirectory(parser)
add_subdirectory(visibility)
include_directories(communication)

include_directories(communication)

target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
#target_link_libraries(simulator liblogog.a)

target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)

#target_link_libraries(simulator libyaml-cpp.a)


 find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(simulator ${Boost_LIBRARIES} pthread)

include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)



configure_file(rob_soc.yaml rob_soc.yaml COPYONLY)