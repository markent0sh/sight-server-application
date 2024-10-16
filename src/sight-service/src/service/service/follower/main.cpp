#include "Logger.hpp"

#include "FollowerAggregator.hpp"

int main() {
  SET_LOG_LEVEL();
  spdlog::set_pattern("[%T %z] [%l] [tid: %t] [%s:%!:%#] - %v");

  SPDLOG_INFO("Follower sight begins");

  auto resolver = std::make_unique<service::follower::resolver::FollowerResolver>(std::move(
    std::make_unique<service::follower::aggregator::FollowerAggregator>()));

  server::follower::FollowerServer server(
    std::string("127.0.0.1"), 44444,
    std::move(resolver));

  auto result = server.Run();

  SPDLOG_INFO("Follower Server exited with code: {}", static_cast<int>(result));

  return 0;
}
