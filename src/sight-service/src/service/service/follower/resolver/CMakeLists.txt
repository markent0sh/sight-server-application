add_library(
  follower-service-resolver
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/FollowerResolver.cpp
)

target_include_directories(
  follower-service-resolver
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  follower-service-resolver
    PUBLIC
      follower-server-public
      follower-server-private
      spdlog::spdlog
)
