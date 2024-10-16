add_executable(logger-test
  main.cpp
)

target_link_libraries(logger-test
  PRIVATE
    sight-common-public
    spdlog::spdlog
)
