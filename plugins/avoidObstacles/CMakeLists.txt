set_property(GLOBAL APPEND PROPERTY agent_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/avoidobstacles_agent_plugin.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/avoidobstacles_parser_plugin.cpp
)

set_property(GLOBAL APPEND PROPERTY viewer_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/avoidobstacles_parser_plugin.cpp

)

set_property(GLOBAL APPEND PROPERTY launcher_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/avoidobstacles_parser_plugin.cpp
)


set_property(GLOBAL APPEND PROPERTY simulator_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/avoidobstacles_parser_plugin.cpp
)


