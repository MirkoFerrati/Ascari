cmake_minimum_required(VERSION 2.6)


find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

set(launcher_MOC_HEADERS mainwindow.h )
set(launcher_SOURCES main.cpp mainwindow.cpp ../shared/yaml_parser.cpp ../shared/yaml_parser_cout.cpp ../viewer/gui/viewer.cpp ../viewer/communication/udp_world_sniffer.cpp ../viewer/gui/Agent.cpp)
SET(launcher_UIS mainwindow.ui)

include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(../viewer/communication)
include_directories(../viewer/gui)
include_directories(../shared/lemon/lemon) #fa schifo, ma non è colpa mia se hanno fatto un progetto configurato di merda

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR})

QT4_WRAP_UI( launcher_UI_HEADERS ${launcher_UIS} )
QT4_WRAP_CPP( launcher_MOC_SOURCES ${launcher_MOC_HEADERS} )


add_executable(launcher
    ${launcher_SOURCES}
    ${launcher_MOC_SOURCES}
    ${launcher_UI_HEADERS}
    ../viewer/communication/udp_world_sniffer.h ../viewer/gui/viewer.h ../viewer/gui/Agent.h
#    ${viewer_RESOURCES_CPP}
)
find_package(Boost REQUIRED system thread serialization)

target_link_libraries(launcher ${Boost_LIBRARIES} pthread)
target_link_libraries(launcher ${QT_LIBRARIES})
target_link_libraries(launcher ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(launcher ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
