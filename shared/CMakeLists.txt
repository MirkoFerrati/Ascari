if(AGENT_LIGHT_FLAG STREQUAL "YES")
add_definitions(-DAGENT_LIGHT=1)
else()
add_definitions(-DAGENT=1)
remove_definitions(-DAGENT_LIGHT)
add_subdirectory(logog)
endif(AGENT_LIGHT_FLAG STREQUAL "YES")



#add_subdirectory(parserYAML)
add_subdirectory(lemon/lemon)
include_directories(../shared/lemon/lemon)
include_directories(yamls)
include_directories(graphs)
#include_directories(yamls/olds)

