#include "Worker.hpp"

#include <functional>
#include <iostream>

int main() {
  // Simple function

  auto void_function = []() {
    std::cout << "Dear manager, I'm working!" << std::endl;
  };

  service::common::worker::Worker<decltype(void_function)> void_worker(
    std::move(void_function),
    std::chrono::milliseconds(100));

  std::cout << "Worker Run result: " << static_cast<int>(void_worker.Run()) << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Worker Stop result: " << static_cast<int>(void_worker.Stop()) << std::endl;

  // Input function

  auto inp_function = [](int value1, const std::string& value2) {
    std::cout << "Dear manager, here are the values: " << value1 << ", " << value2 << std::endl;
  };

  service::common::worker::Worker<decltype(inp_function)> inp_worker(
    std::move(inp_function),
    std::chrono::milliseconds(1000));

  std::cout << "Worker Run result: " << static_cast<int>(inp_worker.Run(12, std::string("Mish"))) << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Worker Stop result: " << static_cast<int>(inp_worker.Stop()) << std::endl;

  // Except function

  auto inp_function_except = [](const int& value1, const std::string& value2) {
    std::cout << "Dear manager, here are the values: " << value1 << ", " << value2 << std::endl;
    static int counter = 0;

    if(counter++ == 7) {
      throw std::invalid_argument("Mishe pora kushatb ;)))");
    }

  };

  service::common::worker::Worker<decltype(inp_function_except)> inp_worker_except(
    std::move(inp_function_except),
    std::chrono::milliseconds(100));

  std::cout << "Worker Run result: " << static_cast<int>(inp_worker_except.Run(12, std::string("Mish"))) << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  std::cout << "Worker Stop result: " << static_cast<int>(inp_worker_except.Stop()) << std::endl;
}
