cmake_minimum_required(VERSION 3.2)

project(
  sight-service
    VERSION 0.0.0
      LANGUAGES CXX
)

set(
  CMAKE_CXX_STANDARD 20
)

set(
  CMAKE_CXX_FLAGS "-Wall -pthread -fPIC -fPIE"
)

include(cmake/dependencies.cmake)

if(NOT DEFINED LOG_LEVEL)
  set(
    LOG_LEVEL "INFO"
  )
endif()

if(NOT DEFINED BUILD_SANDBOX)
  set(BUILD_SANDBOX OFF)
endif()

if(LOG_LEVEL STREQUAL "INFO")
  add_compile_definitions(
    LOG_LEVEL_INFO
  )
elseif(LOG_LEVEL STREQUAL "DEBUG")
  add_compile_definitions(
    LOG_LEVEL_DEBUG
  )
elseif(LOG_LEVEL STREQUAL "VERBOSE")
  add_compile_definitions(
    LOG_LEVEL_TRACE
  )
endif()

message(STATUS "Sandbox build status: ${BUILD_SANDBOX}")
message(STATUS "Log level is set to: ${LOG_LEVEL}")

# cmake .. -DLOG_LEVEL=DEBUG -DBUILD_SANDBOX=ON

add_subdirectory(src)
