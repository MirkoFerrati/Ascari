INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)

include_directories(../shared/lemon/lemon)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h
)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)


CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/lemon.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/lemon.pc
  @ONLY
)

SET(LEMON_SOURCES
  arg_parser.cc
  base.cc
  color.cc
  lp_base.cc
  lp_skeleton.cc
  random.cc
  bits/windows.cc
)



ADD_LIBRARY(lemon ${LEMON_SOURCES})
SET_TARGET_PROPERTIES(lemon PROPERTIES OUTPUT_NAME lemon)



