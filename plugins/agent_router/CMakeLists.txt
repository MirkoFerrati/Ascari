set_property(GLOBAL APPEND PROPERTY agent_SOURCES  
 ${CMAKE_CURRENT_SOURCE_DIR}/graph_creator.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_router.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/agent_router_utility.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/udp_graph_communicator.cpp
)

#message(${agent_SOURCES})
