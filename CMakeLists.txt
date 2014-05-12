cmake_minimum_required(VERSION 2.8)


set (AGENT_LIGHT_FLAG "NO")

project (ascari)
list ( APPEND CMAKE_CXX_FLAGS "-std=c++0x -Wall -pedantic -Wmissing-field-initializers -W -Wunused -Wuninitialized -Wformat=2 -Wctor-dtor-privacy -Wnon-virtual-dtor -Wwrite-strings -Wno-char-subscripts -Wreturn-type -Wcast-qual -Wcast-align -Wsign-promo -Woverloaded-virtual -fno-strict-aliasing  -Werror=address -Werror=parentheses  ${CMAKE_CXX_FLAGS}" )
link_directories(/usr/local/lib)


include_directories(shared/lemon/lemon)
include_directories(simulator)
include_directories(agent)
include_directories(shared)
include_directories(plugins)
add_subdirectory(plugins)

add_subdirectory(agent)
add_subdirectory(gml2lgf)
add_subdirectory(shared)
include_directories(shared)
include_directories(../shared/parserYAML/include)
add_subdirectory(testing)


if (AGENT_LIGHT_FLAG STREQUAL "YES")
message (STATUS "agent light")
add_definitions(-DAGENT_LIGHT=1)

else()
message (STATUS "agent")
remove_definitions(-DAGENT_LIGHT)
add_subdirectory(simulator)
add_subdirectory(launcher)
add_subdirectory(viewer)
include_directories(shared/logog/include)
endif()

add_executable(singleProcess main.cpp)
target_link_libraries(singleProcess simulatorLibrary agentLibrary agent_agent_router simulator_agent_router)

add_executable(results results.cpp)
target_link_libraries(results agent_agent_router ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a boost_system boost_thread boost_serialization boost_program_options)

include_directories( simulator/communication/)
add_executable(viewerReplay viewer_replay.cpp)
target_link_libraries(viewerReplay simulatorLibrary ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a boost_system boost_thread boost_serialization boost_program_options pthread zmq )

#add_subdirectory(yaml_checker)
