#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <optional>
#include <queue>
#include <condition_variable>

#include "Worker.hpp"

namespace service::common::task {

template<typename Task>
class TaskManager {
  public:
    enum class Error {
      eOK = 0,
      eNOK = 1
    };

    TaskManager()
      : tasks_(),
        task_mutex_(),
        exit_mutex_(),
        exiting_(),
        exit_task_(std::make_unique<ExitWorker>(std::bind(&TaskManager::ExitTask, this), std::chrono::milliseconds(1000))),
        exit_condition_(),
        exit_kill_(false) {
      auto result = exit_task_->Run();

      if(ExitWorker::Error::eOK != result) {
        SPDLOG_ERROR("Exit task won't run");
      }

    }

    ~TaskManager() {
      exit_kill_ = true;
    }

    TaskManager(TaskManager&&) = default;
    TaskManager& operator=(TaskManager&&) = default;
    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;

    template<typename... Args>
    Error RunTask(
      const std::string& alias,
      const std::chrono::milliseconds& delay,
      Task&& task, Args&&... args) {
      std::unique_lock<std::mutex> lock(task_mutex_);

      if(tasks_.end() == tasks_.find(alias)) {
        auto result = tasks_.emplace(
          alias, std::make_unique<TaskWorker>(std::move(task), delay));

        if(true == result.second) {
          auto worker_result = result.first->second->Run(std::forward<Args>(args)...);

          if(TaskWorker::Error::eOK != worker_result) {
            SPDLOG_ERROR("Task won't run, alias: {}", alias);
            tasks_.erase(result.first);
            return Error::eNOK;
          } else {
            SPDLOG_DEBUG("Emplaced task with alias: {}, delay: {}, {} tasks running", alias, delay.count(), tasks_.size());
          }

        } else {
          SPDLOG_ERROR("Task won't emplace, alias: {}", alias);
          return Error::eNOK;
        }

      } else {
        SPDLOG_WARN("Task with same alias is already running, alias: {}", alias);
        return Error::eNOK;
      }

      lock.unlock();
      return Error::eOK;
    }

    Error StopTask(const std::string& alias) {
      std::unique_lock<std::mutex> lock(task_mutex_);

      auto result = tasks_.find(alias);

      if(tasks_.end() != result) {
        exiting_.emplace(std::move(result->second));
        exit_condition_.notify_one();

        tasks_.erase(result);
        SPDLOG_DEBUG("Erased task with alias: {}, {} tasks running", alias, tasks_.size());
      } else {
        SPDLOG_ERROR("Task {} not found", alias);
        return Error::eNOK;
      }

      lock.unlock();
      return Error::eOK;
    }

    std::vector<std::string> GetTasks() {
      std::unique_lock<std::mutex> lock(task_mutex_);
      std::vector<std::string> output;
      output.reserve(tasks_.size());

      std::transform(tasks_.begin(), tasks_.end(), std::back_inserter(output), [](const auto& pair) {
        return pair.first;
      });

      return output;
    }

  private:
    void ExitTask() {
      while(false == exit_kill_) {
        std::unique_lock<std::mutex> lock(exit_mutex_);
        exit_condition_.wait(lock, [this]() {
          return false == exiting_.empty();
        });

        SPDLOG_DEBUG("Exiting task, queue size: {}", exiting_.size());

        if(true == exiting_.empty()) {
          continue;
        }

        exiting_.pop();
      }
    }

    using ExitQueueTask = std::function<void(void)>;
    using TaskWorker = service::common::worker::Worker<Task>;
    using ExitWorker = service::common::worker::Worker<ExitQueueTask>;

    std::unordered_map<std::string, std::unique_ptr<TaskWorker>> tasks_;
    std::mutex task_mutex_;
    std::mutex exit_mutex_;
    std::queue<std::unique_ptr<TaskWorker>> exiting_;
    std::unique_ptr<ExitWorker> exit_task_;
    std::condition_variable exit_condition_;
    std::atomic<bool> exit_kill_;
};

} // service::common::task
