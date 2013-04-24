cmake_minimum_required(VERSION 2.6)

include_directories(gui communication)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda
include_directories(../shared/parserYAML/include)

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

set(viewer_HEADERS gui/Agent.h gui/viewer.h communication/udp_world_sniffer.h ../shared/communication/global.h)
set(viewer_SOURCES gui/baseball_viewer.cpp gui/agent_router_viewer.cpp gui/task_assignment_viewer.cpp gui/monitor_viewer.cpp gui/main.cpp gui/Agent.cpp gui/viewer.cpp
communication/udp_world_sniffer.cpp communication/zmq_world_sniffer.hpp communication/zmq_identifier_sniffer.hpp 
../shared/communication/global.cpp ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp ../shared/objects/task_assignment_task.cpp)

#set(viewer_UIS myproject.ui)
#set(viewer_RESOURCES myproject.qrc)

#qt4_wrap_cpp(viewer_MOC ${viewer_HEADERS})
#qt4_wrap_ui(viewer_UIS_H ${viewer_UIS})
#qt4_add_resources(viewer_RESOURCES_CPP ${viewer_RESOURCES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})


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
  
target_link_libraries(viewer ${Boost_LIBRARIES} pthread zmq)
target_link_libraries(viewer ${CMAKE_BINARY_DIR}/shared/lemon/lemon/lemon/liblemon.a)
configure_file(../shared/graphs/test2.gml.lgf test2.gml.lgf COPYONLY)
configure_file(../shared/graphs/test3.gml.lgf test3.gml.lgf COPYONLY)
configure_file(../shared/graphs/test4.gml.lgf test4.gml.lgf COPYONLY)
configure_file(../shared/graphs/test5.gml.lgf test5.gml.lgf COPYONLY)