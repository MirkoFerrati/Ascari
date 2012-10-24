cmake_minimum_required(VERSION 2.6)


find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

set(launcher_MOC_HEADERS mainwindow.h)
set(launcher_SOURCES main.cpp mainwindow.cpp ../shared/yaml_parser.cpp)
SET(launcher_UIS mainwindow.ui)

include_directories(../shared)
include_directories(../shared/logog/include)
include_directories(../shared/parserYAML/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR})

QT4_WRAP_UI( launcher_UI_HEADERS ${launcher_UIS} )
QT4_WRAP_CPP( launcher_MOC_SOURCES ${launcher_MOC_HEADERS} )


add_executable(launcher
    ${launcher_SOURCES}
    ${launcher_MOC_SOURCES}
    ${launcher_UI_HEADERS}
#    ${viewer_RESOURCES_CPP}
)

target_link_libraries(launcher ${QT_LIBRARIES})
target_link_libraries(launcher ${CMAKE_BINARY_DIR}/shared/logog/liblogog.a)
target_link_libraries(launcher ${CMAKE_BINARY_DIR}/shared/parserYAML/libyaml-cpp.a)
