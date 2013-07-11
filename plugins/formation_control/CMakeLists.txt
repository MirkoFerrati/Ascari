
set_property(GLOBAL APPEND PROPERTY agent_SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_agent.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_utility.cpp
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_parser.cpp
)


set_property(GLOBAL APPEND PROPERTY viewer_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_viewer.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_parser.cpp

)

set_property(GLOBAL APPEND PROPERTY launcher_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_viewer.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_parser.cpp
)


set_property(GLOBAL APPEND PROPERTY simulator_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_simulator.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/formation_control_parser.cpp
)

