
set_property(GLOBAL APPEND PROPERTY agent_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/identifier_module.cpp  
	${CMAKE_CURRENT_SOURCE_DIR}/agent_to_dummy_communicator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/dummy_agent.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/identifier_module.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/identifierModule_communicator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/monitor_parser_plugin.cpp

)

set_property(GLOBAL APPEND PROPERTY viewer_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/monitor_viewer.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/monitor_parser_plugin.cpp

)

set_property(GLOBAL APPEND PROPERTY launcher_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/monitor_viewer.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/monitor_parser_plugin.cpp

)

set_property(GLOBAL APPEND PROPERTY simulator_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/monitor_parser_plugin.cpp

)

