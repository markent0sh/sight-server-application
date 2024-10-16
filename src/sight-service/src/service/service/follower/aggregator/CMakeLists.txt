add_library(
  follower-service-aggregator
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/FollowerAggregator.cpp
)

target_include_directories(
  follower-service-aggregator
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  follower-service-aggregator
    PUBLIC
      follower-server-public
		  follower-server-private
      follower-service-resolver
      follower-service-storage
      follower-service-scraper
      service-common-public
      spdlog::spdlog
)
