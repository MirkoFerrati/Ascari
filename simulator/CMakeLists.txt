

add_executable(simulator simulator.cpp main.cpp)

add_subdirectory(parser)
add_subdirectory(visibility)
add_subdirectory(communication)

target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)


include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)

configure_file(monsters.yaml monsters.yaml COPYONLY)