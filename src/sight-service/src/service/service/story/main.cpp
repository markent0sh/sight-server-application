#include "Logger.hpp"

#include "StoryAggregator.hpp"

int main() {
  SET_LOG_LEVEL();
  spdlog::set_pattern("[%T %z] [%l] [tid: %t] [%s:%!:%#] - %v");

  SPDLOG_INFO("Story sight begins");

  auto resolver = std::make_unique<service::story::resolver::StoryResolver>(std::move(
    std::make_unique<service::story::aggregator::StoryAggregator>()));

  server::story::StoryServer server(
    std::string("127.0.0.1"), 33333,
    std::move(resolver));

  auto result = server.Run();

  SPDLOG_INFO("Story Server exited with code: {}", static_cast<int>(result));

  return 0;
}
