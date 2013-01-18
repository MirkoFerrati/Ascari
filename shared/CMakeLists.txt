

add_subdirectory(logog)
add_subdirectory(parserYAML)
add_subdirectory(lemon/lemon)

include_directories(../shared/lemon/lemon)
include_directories(yamls)
include_directories(graphs)
include_directories(yamls/olds)
include_directories(bilp_problem)
add_subdirectory(bilp_problem)