add_executable(follower-sight-service
  main.cpp
)

target_link_libraries(follower-sight-service
  PRIVATE
    spdlog::spdlog
    follower-service-aggregator
    service-common-public
    sight-common-public
)

add_subdirectory(scraper)
add_subdirectory(storage)
add_subdirectory(resolver)
add_subdirectory(aggregator)
