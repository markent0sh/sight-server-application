add_library(
  story-service-resolver
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/StoryResolver.cpp
)

target_include_directories(
  story-service-resolver
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  story-service-resolver
    PUBLIC
      story-server-public
      story-server-private
      spdlog::spdlog
)
