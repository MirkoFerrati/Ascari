cmake_minimum_required(VERSION 2.8)

set(agent_SOURCES ../agent/empty_plugin/empty_plugin.cpp 
 ../agent/graph_routing/graph_creator.cpp ../agent/encoder/encoderDet.cpp ../agent/automaton/automatonFSM.cpp 
  ../agent/communication/udp_agent_communicator.cpp ../agent/identifierModule/identifier_module.cpp ../shared/dynamic.cpp
  ../agent/communication/udp_world_communicator.cpp ../agent/controller/controller.cpp ../agent/agent.cpp 
  ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp ../agent/decoder/decoder.cpp 
  ../agent/graph_routing/agent_router.cpp ../agent/graph_routing/agent_router_utility.cpp ../agent/graph_routing/udp_graph_communicator.cpp
	../shared/communication/zmq_full_communicator.hpp ../shared/communication/global.cpp
	../shared/communication/zmq_communicator.hpp ../agent/communication/zmq_world_communicator.cpp
	../agent/automaton/automatonEFSM.cpp
)

set(checker_SOURCES ../shared/yaml_checker/yaml_checker.cpp main.cpp)

set(simulator_SOURCES ../simulator/collisionchecker.cpp ../shared/dynamic.cpp 
../simulator/communication/udp_agent_communicator.cpp ../simulator/simulator.cpp 
../simulator/communication/agent_communicator_abstract.h ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp 
../simulator/communication/zmq_agent_communicator.cpp ../shared/communication/global.cpp ../simulator/visibility/visibility.cpp
	../shared/communication/zmq_full_communicator.hpp ../shared/communication/zmq_communicator.hpp ../simulator/communication/task_assignment_router.hpp
)

FIND_PACKAGE(GLPK 4.33)

set(GLPK_FOUND 0)
if (${GLPK_FOUND})
add_definitions(-DGLPK_FOUND)
#written by Alessandro Settimi
set (agent_SOURCES ${agent_SOURCES} task_assignment/task_assignment.cpp task_assignment/task_assignment_utility.cpp task_assignment/algorithms_utility.cpp task_assignment/solution_exchange_algorithm.cpp task_assignment/cost_exchange_algorithm.cpp task_assignment/subgradient_algorithm.cpp task_assignment/bilp_problem.cpp)
#written by Alessandro Settimi
endif (${GLPK_FOUND})

include_directories(identifierModule)
include_directories(controller)
include_directories(encoder)
include_directories(automaton)
include_directories(../agent/communication)
include_directories(topology)
include_directories(../simulator/communication)
add_executable(checker ${agent_SOURCES} ${checker_SOURCES} ${simulator_SOURCES})

target_link_libraries(checker ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(checker ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
target_link_libraries(checker ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)
target_link_libraries(checker zmq)

if (${GLPK_FOUND})
#written by Alessandro Settimi
target_link_libraries(checker glpk)
#written by Alessandro Settimi
endif (${GLPK_FOUND})

include_directories(../agent)
include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(decoder)
include_directories(graph_routing)
#written by Alessandro Settimi
include_directories(task_assignment)
#written by Alessandro Settimi
include_directories(empty_plugin)
include_directories(../shared/boost/geometry)
include_directories(../shared/boost/geometry/*)

find_package(Boost REQUIRED system thread serialization)


target_link_libraries(checker ${Boost_LIBRARIES} pthread)


message("Configurazione di ${CMAKE_BUILD_TYPE}")
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/checker" "../shared/yamls/*.yaml")
FOREACH(infileName ${yaml_files})
MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})

configure_file(../shared/graphs/test2.gml.lgf test2.gml.lgf COPYONLY)
configure_file(../shared/graphs/test3.gml.lgf test3.gml.lgf COPYONLY)
configure_file(../shared/graphs/test4.gml.lgf test4.gml.lgf COPYONLY)
configure_file(../shared/graphs/test5.gml.lgf test5.gml.lgf COPYONLY)

