add_library(
  story-service-storage
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/StoryStorage.cpp
)

target_include_directories(
  story-service-storage
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries(
  story-service-storage
    PUBLIC
      story-server-public
      sight-storage-mongo
      spdlog::spdlog
)
