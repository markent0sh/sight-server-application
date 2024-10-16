add_library(
  follower-service-storage
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/FollowerStorage.cpp
)

target_include_directories(
  follower-service-storage
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  follower-service-storage
    PUBLIC
      follower-server-public
      sight-storage-mongo
      spdlog::spdlog
)
