cmake_minimum_required(VERSION 2.8)
add_definitions(-DISVIEWER)

include_directories(gui communication)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(../shared/parserYAML/include)
include_directories(../shared/)
include_directories(../shared/logog/include)

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})


set(viewer_HEADERS gui/Agent.h gui/viewer.h ../shared/communication/global.h)
set_property(GLOBAL APPEND PROPERTY viewer_SOURCES gui/baseball_viewer.cpp 
 gui/main.cpp gui/Agent.cpp gui/viewer.cpp ../shared/define.cpp
 communication/zmq_world_sniffer.hpp  ../shared/objects/router_graph_good.cpp
../simulator/visibility/map2d.cpp ../gml2lgf/utils.cpp ../gml2lgf/gmlreader.hpp  ../gml2lgf/node.cpp ../gml2lgf/graph.cpp ../gml2lgf/arc.cpp
../shared/communication/global.cpp ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp ../shared/objects/task_assignment_task.cpp)

#set(viewer_UIS myproject.ui)
#set(viewer_RESOURCES myproject.qrc)

#qt4_wrap_cpp(viewer_MOC ${viewer_HEADERS})
#qt4_wrap_ui(viewer_UIS_H ${viewer_UIS})
#qt4_add_resources(viewer_RESOURCES_CPP ${viewer_RESOURCES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

get_property(viewer_SOURCES GLOBAL PROPERTY viewer_SOURCES)

add_executable(viewer
    ${viewer_SOURCES}
    ${viewer_MOC}
#    ${viewer_UIS_H}
#    ${viewer_RESOURCES_CPP}
)

target_link_libraries(viewer ${QT_LIBRARIES})
target_link_libraries(viewer ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
 find_package(Boost REQUIRED system thread serialization)
  
  target_link_libraries(viewer ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
  
target_link_libraries(viewer boost_system boost_thread boost_serialization boost_program_options pthread zmq)
target_link_libraries(viewer ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)
file(GLOB yaml_files RELATIVE  "${CMAKE_SOURCE_DIR}/agent" "../shared/graphs/*.lgf")
FOREACH(infileName ${yaml_files})
MESSAGE(STATUS "Process file: ${infileName}")
configure_file(${infileName} ./ COPYONLY)
endforeach(${infileName})