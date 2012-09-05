

add_executable(simulator ../shared/dynamic.cpp 
communication/udp_agent_communicator.cpp simulator.cpp main.cpp 
communication/agent_communicator_abstract.h ../shared/yaml_parser.cpp)

add_subdirectory(parser)
add_subdirectory(visibility)
include_directories(communication)

include_directories(communication)

target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
#target_link_libraries(simulator liblogog.a)

target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)

#target_link_libraries(simulator libyaml-cpp.a)


 find_package(Boost REQUIRED system thread serialization)
 
target_link_libraries(simulator ${Boost_LIBRARIES} pthread)
target_link_libraries(simulator ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)


include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda

message("Configurazione di ${CMAKE_BUILD_TYPE}")
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/simulator" "../shared/yamls/*.yaml")
FOREACH(infileName ${yaml_files})
MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})

configure_file(../shared/graphs/test2.gml.lgf test2.gml.lgf COPYONLY)
