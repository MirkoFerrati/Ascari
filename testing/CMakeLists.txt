cmake_minimum_required(VERSION 2.8)


add_executable(fake_simulator fake_simulator.cpp main.cpp ../shared/communication/global.cpp ../shared/yaml_parser.cpp)

target_link_libraries(fake_simulator ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
target_link_libraries(fake_simulator zmq)
target_link_libraries(fake_simulator lemon)

include_directories(../shared)
include_directories(../shared/parserYAML/include)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(plugins)

find_package(Boost COMPONENTS REQUIRED system thread serialization program_options)

target_link_libraries(fake_simulator boost_system boost_thread boost_serialization boost_program_options pthread)
target_link_libraries(fake_simulator serial)


#message("Configurazione di ${CMAKE_BUILD_TYPE}")
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/testing" "../shared/yamls/*.yaml")
FOREACH(infileName ${yaml_files})
#MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})


