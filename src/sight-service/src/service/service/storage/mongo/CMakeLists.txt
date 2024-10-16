add_library(
  sight-storage-mongo
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/MongoClient.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/MongoObject.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/MongoCreator.cpp
)

target_include_directories(
  sight-storage-mongo
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  sight-storage-mongo
    PUBLIC
      sight-storage-public
      mongo::mongocxx_shared
      mongo::bsoncxx_shared
      service-common-public
      spdlog::spdlog
)
