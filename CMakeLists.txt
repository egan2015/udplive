cmake_minimum_required(VERSION 2.8)
project(udplive)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in"
  "${PROJECT_SOURCE_DIR}/include/config.h"
  )
   OPTION(CONFIG_BIT_PRE_LONG "help string describing option"
         32)
include(CheckTypeSize)
CHECK_TYPE_SIZE("unsigned long" BIT_PRE_LONG)
if (NOT CONFIG_BIT_PRE_LONG)
  set(CONFIG_BIT_PRE_LONG ${BIT_PRE_LONG})
  MESSAGE(STATUS "unsigned long type size : ${BIT_PRE_LONG}")
endif()


add_subdirectory(testProgs bin)
add_subdirectory(vlstp lib)

