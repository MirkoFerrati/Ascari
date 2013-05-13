
#TODO: add plugin subdir

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})
add_subdirectory(task_assignment)
add_subdirectory(agent_router)
add_subdirectory(monitor)