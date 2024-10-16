#pragma once

#include <thread>
#include <atomic>
#include <utility>
#include <chrono>

#include "Logger.hpp"

namespace service::common::worker {

template<typename Task>
class Worker {
  public:
    enum class Error {
      eOK = 0,
      eNOK = 1
    };

    explicit Worker(
      Task&& task,
      std::chrono::milliseconds delay)
      : task_(std::move(task)),
        delay_(delay) {
    }

    ~Worker() {
      Stop();
    }

    Worker(Worker&&) = default;
    Worker& operator=(Worker&&) = default;
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    template<typename... Args>
    Error Run(Args&&... args) {
      Error error = Error::eOK;

      if(false == worker_.joinable()) {
        auto tuple = std::make_shared<
          std::tuple<Args...>>(
            std::make_tuple(std::forward<Args>(args)...));
        worker_ = std::thread([tuple, this]() {
          while(false == stop_) {
            try {
              std::apply([this](auto&&... unpacked_args) {
                  task_(std::forward<decltype(unpacked_args)>(unpacked_args)...);
              }, *tuple);
            } catch(...) {
              SPDLOG_ERROR("Oops, exception from task");
            }

            std::this_thread::sleep_for(delay_);
          }
        });
      } else {
        SPDLOG_WARN("Already running");
        error = Error::eNOK;
      }

      return error;
    }

    Error Stop() {
      SPDLOG_DEBUG("Stop");
      Error error = Error::eOK;

      stop_ = true;

      if(true == worker_.joinable()) {
        worker_.join();
        SPDLOG_DEBUG("Stopped");
      } else {
        SPDLOG_WARN("No joinable thread");
        error = Error::eNOK;
      }

      return error;
    }
  private:
    Task task_;
    std::chrono::milliseconds delay_;
    std::thread worker_;
    std::atomic<bool> stop_;
};

} // service::common::worker
