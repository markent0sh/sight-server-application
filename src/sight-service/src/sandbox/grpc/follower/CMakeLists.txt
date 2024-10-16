add_executable(follower-service-test
  main.cpp
)

target_link_libraries(follower-service-test
  PRIVATE
    follower-server-public
    follower-server-private
)
