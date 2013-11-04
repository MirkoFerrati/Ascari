cmake_minimum_required(VERSION 2.8)


if(AGENT_LIGHT_FLAG STREQUAL "YES")
add_definitions(-DAGENT_LIGHT=1)
else()
add_definitions(-DAGENT=1)
endif()

include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(plugins)

# add_executable(test testrandom.hpp main.cpp ../agent/controller/controller.cpp ../agent/decoder/decoder.cpp 
# ../shared/dynamic.cpp ../agent/encoder/encoderDet.cpp ../agent/automaton/automatonFSM.cpp ../simulator/visibility/visibility.cpp
# ../simulator/visibility/map2d.cpp ../gml2lgf/graph.cpp ../gml2lgf/node.cpp ../gml2lgf/arc.cpp ../gml2lgf/gmlreader.hpp
# ../shared/objects/router_graph_good.cpp
# 	../shared/objects/task_assignment_task.cpp)
# add_executable(test_udp_agent  ../shared/yaml_parser.cpp fake_agent_for_udp_tester.cpp ../agent/communication/udp_agent_communicator.cpp ../shared/communication/global.cpp)
 find_package(Boost REQUIRED system thread serialization)
 include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda

add_executable(fake_localization fake_localization.cpp ../shared/define.cpp ../shared/communication/zmq_localization_communicator.hpp ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp ../shared/communication/global.cpp)
#add_executable(fake_simulator fake_simulator.cpp main.cpp ../shared/communication/global.cpp ../shared/yaml_parser.cpp zmq_agent_communicator.cpp ../shared/dynamic.cpp
#../shared/communication/zmq_localization_communicator.hpp ../shared/dynamic_remote_localization.cpp ../simulator/visibility/visibility.cpp ../shared/objects/objects_container.hpp)
#target_link_libraries(fake_simulator ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
#target_link_libraries(fake_simulator zmq)
#target_link_libraries(fake_simulator lemon)
#target_link_libraries(fake_simulator ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
#target_link_libraries(fake_simulator boost_system boost_thread boost_serialization boost_program_options pthread)


target_link_libraries(fake_localization zmq boost_system boost_thread boost_serialization boost_program_options pthread ) 
target_link_libraries(fake_localization ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)
target_link_libraries(fake_localization ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
target_link_libraries(fake_localization ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)



#message("Configurazione di ${CMAKE_BUILD_TYPE}")
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/testing" "../shared/yamls/*.yaml")
FOREACH(infileName ${yaml_files})
#MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})
