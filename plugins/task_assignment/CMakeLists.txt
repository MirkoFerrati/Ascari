set(GLPK_FOUND 1)
if (${GLPK_FOUND})
add_definitions(-DGLPK_FOUND)
#written by Alessandro Settimi
set_property(GLOBAL APPEND PROPERTY agent_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/task_assignment.cpp ${CMAKE_CURRENT_SOURCE_DIR}/task_assignment_utility.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/algorithms_utility.cpp ${CMAKE_CURRENT_SOURCE_DIR}/subgradient_algorithm.cpp
${CMAKE_CURRENT_SOURCE_DIR}/assignment_problem.cpp ${CMAKE_CURRENT_SOURCE_DIR}/dubins.cpp)  
#written by Alessandro Settimi
endif (${GLPK_FOUND})


set_property(GLOBAL APPEND PROPERTY viewer_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/task_assignment_viewer.cpp  
)

set_property(GLOBAL APPEND PROPERTY launcher_SOURCES  
${CMAKE_CURRENT_SOURCE_DIR}/task_assignment_viewer.cpp  
)

