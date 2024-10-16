add_library(
  story-service-aggregator
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/StoryAggregator.cpp
)

target_include_directories(
  story-service-aggregator
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  story-service-aggregator
    PUBLIC
      story-server-public
		  story-server-private
      story-service-resolver
      story-service-storage
      story-service-scraper
      service-common-public
      spdlog::spdlog
)
