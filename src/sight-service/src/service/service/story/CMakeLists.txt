add_executable(story-sight-service
  main.cpp
)

target_link_libraries(story-sight-service
  PRIVATE
    spdlog::spdlog
    story-service-aggregator
    service-common-public
    sight-common-public
)

add_subdirectory(scraper)
add_subdirectory(storage)
add_subdirectory(resolver)
add_subdirectory(aggregator)
