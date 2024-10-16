add_library(
  follower-service-scraper
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/FollowerScraperClient.cpp
)

target_include_directories(
  follower-service-scraper
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  follower-service-scraper
    PUBLIC
      service-common-public
      spdlog::spdlog
    PRIVATE
      cpr::cpr
      nlohmann_json::nlohmann_json
)
