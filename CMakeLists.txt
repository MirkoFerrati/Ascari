project (ascari)

add_subdirectory(agent)
add_subdirectory(simulator)

add_subdirectory(shared)

include_directories(shared)
include_directories(shared/logog/include)

add_definitions(-Wall -pedantic)


add_subdirectory(viewer)

