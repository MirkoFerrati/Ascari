set_property(GLOBAL APPEND PROPERTY agent_SOURCES  
 ${CMAKE_CURRENT_SOURCE_DIR}/graph_creator.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_router.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_router_utility.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/udp_graph_communicator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_router_parser_plugin.cpp
)

#message(${agent_SOURCES})
set_property(GLOBAL APPEND PROPERTY viewer_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/agent_router_viewer.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/agent_router_parser_plugin.cpp
)

set_property(GLOBAL APPEND PROPERTY launcher_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/agent_router_viewer.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/agent_router_parser_plugin.cpp
)

set_property(GLOBAL APPEND PROPERTY simulator_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/agent_router_simulator_plugin.cpp
${CMAKE_CURRENT_SOURCE_DIR}/agent_router_parser_plugin.cpp
)

include_directories(../../shared/lemon)