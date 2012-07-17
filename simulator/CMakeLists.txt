

add_executable(simulator communication/udp_agent_router.cpp ../shared/dynamic.cpp communication/udp_agent_communicator.cpp simulator.cpp main.cpp communication/agent_communicator_abstract.h ../shared/yaml_parser.cpp)

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

include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)

configure_file(../shared/esempio1.yaml esempio1.yaml COPYONLY)
configure_file(../shared/esempio2.yaml esempio2.yaml COPYONLY)
configure_file(../shared/esempio3.yaml esempio3.yaml COPYONLY)
configure_file(../shared/esempio4.yaml esempio4.yaml COPYONLY)
configure_file(../shared/esempio5.yaml esempio5.yaml COPYONLY)
configure_file(../shared/esempio6.yaml esempio6.yaml COPYONLY)
configure_file(../shared/esempio7.yaml esempio7.yaml COPYONLY)
configure_file(../shared/esempio9.yaml esempio9.yaml COPYONLY)
configure_file(../shared/test2.gml.lgf test2.gml.lgf COPYONLY)
configure_file(../shared/baseball.yaml baseball.yaml COPYONLY)
configure_file(../shared/giocopalla.yaml giocopalla.yaml COPYONLY)