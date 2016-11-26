cmake_minimum_required(VERSION 2.8)
project(udplive)

include(CheckTypeSize)
CHECK_TYPE_SIZE("unsigned long" BIT_PRE_LONG)
MESSAGE(STATUS "unsigned long type size : ${BIT_PRE_LONG}")
if (BIT_PRE_LONG EQUAL 8)
  set(CONFIG_BIT_PRE_LONG 64)
else()
  set(CONFIG_BIT_PRE_LONG 32)
endif()

configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in"
  "${PROJECT_SOURCE_DIR}/include/config.h"
  )

add_subdirectory(testProgs bin)
add_subdirectory(vlstp lib)

