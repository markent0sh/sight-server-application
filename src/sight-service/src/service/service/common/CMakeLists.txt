add_library(
  service-common-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/manager/TaskManager.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/worker/Worker.hpp
)

target_include_directories(
  service-common-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/worker/
      ${CMAKE_CURRENT_SOURCE_DIR}/manager/
      service-common-public
)

target_link_libraries(
  service-common-public
    INTERFACE
      sight-common-public
)
