// Logging
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include "spdlog/spdlog.h"

#include "TaskManager.hpp"

int main() {
  SPDLOG_INFO("Sight begins");

  auto arg_function = [](
    const std::string& name, int value) {
    SPDLOG_INFO("Dear {}, your value is {}", name, value);
  };

  service::common::task::TaskManager<
    decltype(arg_function)> arg_worker;

  auto error = arg_worker.RunTask(
    std::string("mish"),
    std::chrono::milliseconds(1000),
    std::move(arg_function),
    std::string("Mish"),
    52);

  SPDLOG_INFO("Task is runnning, error: {}", static_cast<int>(error));

  error = arg_worker.RunTask(
    std::string("mish"),
    std::chrono::milliseconds(1000),
    std::move(arg_function),
    std::string("Mish"),
    53);

  SPDLOG_INFO("Task is runnning, error: {}", static_cast<int>(error));

  std::this_thread::sleep_for(std::chrono::seconds(2));
  arg_worker.StopTask(std::string("mish"));

  return 0;
}
