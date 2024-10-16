add_library(
  story-service-scraper
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/StoryScraperClient.cpp
)

target_include_directories(
  story-service-scraper
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  story-service-scraper
    PUBLIC
      service-common-public
      spdlog::spdlog
    PRIVATE
      cpr::cpr
      nlohmann_json::nlohmann_json
)
