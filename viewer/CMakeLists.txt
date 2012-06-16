cmake_minimum_required(VERSION 2.6)

include_directories(gui communication)

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

set(viewer_HEADERS gui/Agent.h gui/AgentPath.h gui/filereader.h gui/parser.h gui/TargetList.h gui/wall.h gui/wallreader.h gui/viewer.h)
set(viewer_SOURCES gui/main.cpp gui/Agent.cpp gui/filereader.cpp gui/parser.cpp gui/viewer.cpp gui/wallreader.cpp)
#set(viewer_UIS myproject.ui)
#set(viewer_RESOURCES myproject.qrc)

qt4_wrap_cpp(viewer_MOC ${viewer_HEADERS})
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