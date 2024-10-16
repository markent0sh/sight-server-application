add_executable(worker-test-service
  worker.cpp
)

target_link_libraries(worker-test-service
  PRIVATE
    service-common-public
    spdlog::spdlog
)

add_executable(manager-test-service
  manager.cpp
)

target_link_libraries(manager-test-service
  PRIVATE
    service-common-public
    spdlog::spdlog
)
