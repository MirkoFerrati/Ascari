


add_executable(agent graph_routing/graph_creator.cpp encoder/encoderDet.cpp automaton/automatonFSM.cpp communication/udp_agent_communicator.cpp identifierModule/identifier_module.cpp
					communication/udp_world_communicator.cpp controller/controller.cpp agent.cpp main.cpp
			  ../shared/yaml_parser.cpp decoder/decoder.cpp graph_routing/agent_router.cpp graph_routing/udp_graph_communicator.cpp)




include_directories(identifierModule)
include_directories(controller)
include_directories(encoder)
include_directories(automaton)
include_directories(communication)
include_directories(topology)

target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)

include_directories(../agent)
include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(decoder)
include_directories(graph_routing)
include_directories(../shared/boost/geometry)
include_directories(../shared/boost/geometry/*)

find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(agent ${Boost_LIBRARIES} pthread)


message("Configurazione di ${CMAKE_BUILD_TYPE}")
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/agent" "../shared/yamls/*.yaml")
FOREACH(infileName ${yaml_files})
MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})

configure_file(../shared/graphs/test2.gml.lgf test2.gml.lgf COPYONLY)
configure_file(../shared/graphs/test3.gml.lgf test3.gml.lgf COPYONLY)
