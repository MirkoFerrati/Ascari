


add_executable(agent encoder/encoderDet.cpp automaton/automatonFSM.cpp communication/udp_agent_communicator.cpp identifierModule/identifier_module.cpp
					communication/udp_world_communicator.cpp controller/controller.cpp agent.cpp main.cpp
			  ../shared/yaml_parser.cpp decoder/decoder.cpp )


include_directories(identifierModule)
include_directories(controller)
include_directories(encoder)
include_directories(automaton)
include_directories(communication)
include_directories(topology)

target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)

include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(decoder)
include_directories(graph_routing)

 find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(agent ${Boost_LIBRARIES} pthread)

message(${CMAKE_BUILD_TYPE})

configure_file(../shared/esempio1.yaml esempio1.yaml COPYONLY)
configure_file(../shared/esempio2.yaml esempio2.yaml COPYONLY)
configure_file(../shared/esempio3.yaml esempio3.yaml COPYONLY)
configure_file(../shared/esempio4.yaml esempio4.yaml COPYONLY)
configure_file(../shared/esempio5.yaml esempio5.yaml COPYONLY)