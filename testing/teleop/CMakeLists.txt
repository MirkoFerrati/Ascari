cmake_minimum_required(VERSION 2.8)

link_directories(/usr/local/lib)

list ( APPEND CMAKE_CXX_FLAGS "-std=c++0x -Wall -pedantic -Wmissing-field-initializers -W -Wunused -Wuninitialized -Wformat=2 -Wctor-dtor-privacy -Wnon-virtual-dtor -Wwrite-strings -Wno-char-subscripts -Wreturn-type -Wcast-qual -Wcast-align -Wsign-promo -Woverloaded-virtual -fno-strict-aliasing  -Werror=address -Werror=parentheses  ${CMAKE_CXX_FLAGS}" )


set_property(GLOBAL APPEND PROPERTY SOURCES 
	serial_communicator.hpp main.cpp
        raw_serial.hpp
)


get_property(sources GLOBAL PROPERTY SOURCES)
add_executable(teleop ${sources})

find_package(Boost COMPONENTS REQUIRED system thread serialization program_options)

target_link_libraries(teleop boost_system boost_thread boost_serialization boost_program_options pthread)
target_link_libraries(teleop serial)



