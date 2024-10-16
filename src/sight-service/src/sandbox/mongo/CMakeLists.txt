add_executable(mongo-test
  main.cpp
)

target_link_libraries(mongo-test
  PRIVATE
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
    service-common-public
    spdlog::spdlog
)

add_executable(mongo-test-service
  mongo-test.cpp
)

target_link_libraries(mongo-test-service
  PRIVATE
    sight-storage-mongo
    service-common-public
    spdlog::spdlog
)
