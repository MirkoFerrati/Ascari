cmake_minimum_required(VERSION 2.8)

add_definitions(-DISAGENT)

set_property(GLOBAL APPEND PROPERTY agent_SOURCES 
  ../shared/dynamic.cpp agent.cpp main.cpp
  ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp  
	../shared/communication/zmq_full_communicator.hpp ../shared/communication/global.cpp
	../shared/communication/zmq_communicator.hpp communication/zmq_world_communicator.cpp 
	communication/zmq_real_world_serial_communicator.cpp
	../shared/objects/task_assignment_task.cpp ../shared/objects/router_graph_good.cpp
)

#add_definitions(-DZMQDEBUG)

get_property(sources GLOBAL PROPERTY agent_SOURCES)
add_executable(agent ${sources})

target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)
target_link_libraries(agent zmq)

include_directories(../agent)
include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(plugins)

find_package(Boost COMPONENTS REQUIRED system thread serialization program_options)

target_link_libraries(agent boost_system boost_thread boost_serialization boost_program_options pthread)

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

