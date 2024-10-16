add_executable(story-service-test
  main.cpp
)

target_link_libraries(story-service-test
  PRIVATE
    story-server-public
    story-server-private
)
