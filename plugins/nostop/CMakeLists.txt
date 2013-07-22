set(GLPK_FOUND 1)
if (${GLPK_FOUND})
add_definitions(-DGLPK_FOUND)
set_property(GLOBAL APPEND PROPERTY agent_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/nostop_agent_plugin.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nostop_box.hpp ${CMAKE_CURRENT_SOURCE_DIR}/nostop_communicator.hpp 
${CMAKE_CURRENT_SOURCE_DIR}/nostop_agent_position.cpp ${CMAKE_CURRENT_SOURCE_DIR}/nostop_discretized_area.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nostop_ISLAlgorithm.cpp ${CMAKE_CURRENT_SOURCE_DIR}/nostop_line2D.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nostop_math.cpp ${CMAKE_CURRENT_SOURCE_DIR}/nostop_parser_plugin.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nostop_plugin.hpp ${CMAKE_CURRENT_SOURCE_DIR}/nostop_probability.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nostop_real2D.cpp ${CMAKE_CURRENT_SOURCE_DIR}/nostop_router.hpp
${CMAKE_CURRENT_SOURCE_DIR}/nostop_thread.hpp ${CMAKE_CURRENT_SOURCE_DIR}/nostop_agent_id.cpp 
)
endif (${GLPK_FOUND})

set_property(GLOBAL APPEND PROPERTY viewer_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/nostop_viewer_plugin.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/nostop_parser_plugin.cpp

)

set_property(GLOBAL APPEND PROPERTY launcher_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/nostop_viewer_plugin.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/nostop_parser_plugin.cpp

)

set_property(GLOBAL APPEND PROPERTY simulator_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/nostop_simulator_plugin.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/nostop_router.hpp
${CMAKE_CURRENT_SOURCE_DIR}/nostop_parser_plugin.cpp
)

