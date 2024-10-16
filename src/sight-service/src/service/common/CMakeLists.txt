add_library(
  sight-common-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/logging/Logger.hpp
)

target_include_directories(
  sight-common-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/logging/
)
