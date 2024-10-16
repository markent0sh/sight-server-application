add_library(
  sight-storage-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/StorageClient.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/StorageObject.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/StorageTypes.hpp
)

target_include_directories(
  sight-storage-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/
)
