link_directories(/usr/local/lib)

set_property(GLOBAL APPEND PROPERTY simulator_SOURCES collisionchecker.cpp ../shared/dynamic.cpp 
communication/udp_agent_communicator.cpp simulator.cpp main.cpp communication/zmq_viewer_communicator.hpp
communication/agent_communicator_abstract.h ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp 
communication/zmq_agent_communicator.cpp ../shared/communication/global.cpp visibility/visibility.cpp
../shared/objects/router_graph_good.cpp ../shared/dynamic_remote_localization.cpp
	../shared/communication/zmq_full_communicator.hpp ../shared/communication/zmq_communicator.hpp 
	../shared/objects/task_assignment_task.cpp visibility/map2d.cpp ../gml2lgf/graph.cpp ../gml2lgf/node.cpp ../gml2lgf/arc.cpp ../gml2lgf/gmlreader.hpp
)

add_definitions(-DISSIMULATOR)
get_property(sources GLOBAL PROPERTY simulator_SOURCES)

add_executable(simulator ${sources})

add_subdirectory(parser)
include_directories(visibility)
include_directories(communication)

include_directories(communication)

target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
#target_link_libraries(simulator liblogog.a)

target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)

#target_link_libraries(simulator libyaml-cpp.a)

target_link_libraries(simulator zmq)

 find_package(Boost 1.49.0 REQUIRED system thread serialization)
 
target_link_libraries(simulator pthread)
target_link_libraries(simulator boost_system boost_thread boost_serialization boost_program_options boost_regex)
target_link_libraries(simulator rt)


include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)

message("Configurazione di ${CMAKE_BUILD_TYPE}")
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/simulator" "../shared/yamls/*.yaml")
FOREACH(infileName ${yaml_files})
#MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})

configure_file(../shared/graphs/museum.gml museum.gml COPYONLY)
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/agent" "../shared/graphs/*.lgf")
FOREACH(infileName ${yaml_files})
#MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})
