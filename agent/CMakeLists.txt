cmake_minimum_required(VERSION 2.8)



set_property(GLOBAL APPEND PROPERTY agent_SOURCES 
  encoder/encoderDet.cpp automaton/automatonFSM.cpp automaton/automatonEFSM.cpp
  communication/udp_agent_communicator.cpp  ../shared/dynamic.cpp
  communication/udp_world_communicator.cpp controller/controller.cpp agent.cpp main.cpp
  ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp decoder/decoder.cpp 
	../shared/communication/zmq_full_communicator.hpp ../shared/communication/global.cpp
	../shared/communication/zmq_communicator.hpp communication/zmq_world_communicator.cpp 
	../shared/objects/task_assignment_task.cpp ../shared/objects/router_graph_good.cpp
)
#set(agent_SOURCES ${agent_SOURCES} )

#add_definitions(-DZMQDEBUG)

#FIND_PACKAGE(GLPK 4.33)



get_property(sources GLOBAL PROPERTY agent_SOURCES)
add_executable(agent ${sources})

include_directories(identifierModule)
include_directories(controller)
include_directories(encoder)
include_directories(automaton)
include_directories(communication)
include_directories(topology)

target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)
target_link_libraries(agent zmq)

#written by Alessandro Settimi
target_link_libraries(agent glpk)
#written by Alessandro Settimi

include_directories(../agent)
include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(decoder)
include_directories(plugins)
#written by Alessandro Settimi
include_directories(task_assignment)
#written by Alessandro Settimi
include_directories(empty_plugin)
include_directories(../shared/boost/geometry)
include_directories(../shared/boost/geometry/*)

find_package(Boost REQUIRED system thread serialization)


target_link_libraries(agent ${Boost_LIBRARIES} pthread)


#message("Configurazione di ${CMAKE_BUILD_TYPE}")
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/agent" "../shared/yamls/*.yaml")
FOREACH(infileName ${yaml_files})
#MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})

file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/agent" "../shared/graphs/*.lgf")
FOREACH(infileName ${yaml_files})
#MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})

configure_file(test.sh test.sh COPYONLY)

