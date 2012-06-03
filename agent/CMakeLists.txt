


add_executable(agent communication/udp_agent_communicator.cpp identifierModule/identifier_module.cpp
					communication/udp_world_communicator.cpp controller/controller.cpp agent.cpp main.cpp)


add_subdirectory(identifierModule)
add_subdirectory(controller)
add_subdirectory(encoder)
add_subdirectory(automaton)
include_directories(communication)
add_subdirectory(topology)

target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(agent ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)

include_directories(../shared)
include_directories(../shared/logog/include)
add_subdirectory(decoder)
add_subdirectory(graph_routing)

 find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(agent ${Boost_LIBRARIES} pthread)

message(${CMAKE_BUILD_TYPE})