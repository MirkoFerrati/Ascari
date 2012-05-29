


add_executable(agent communication/dummy_world_communicator.cpp communication/shared_agent_communicator.cpp communication/tcp_agent_communicator.cpp communication/shared_world_communicator.cpp communication/tcp_world_communicator.cpp identifierModule/identifier.cpp controller/controller.cpp agent.cpp main.cpp)


add_subdirectory(identifierModule)
add_subdirectory(controller)
add_subdirectory(encoder)
add_subdirectory(automaton)
add_subdirectory(communication)
add_subdirectory(topology)

include_directories(../shared)
include_directories(../shared/logog/include)
add_subdirectory(decoder)
add_subdirectory(graph_routing)

message(${CMAKE_BUILD_TYPE})