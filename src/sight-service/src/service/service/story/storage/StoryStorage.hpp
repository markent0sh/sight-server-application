#pragma once

#include <unordered_map>

#include "StoryServer.hpp"
#include "StorageClient.hpp"
#include "StorageCreator.hpp"

namespace service::story::storage {

class StoryStorage {
  public:
    StoryStorage(
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IObjectCreator>);
    ~StoryStorage() = default;
    StoryStorage(StoryStorage&&) = default;
    StoryStorage& operator=(StoryStorage&&) = default;
    StoryStorage(const StoryStorage&) = delete;
    StoryStorage& operator=(const StoryStorage&) = delete;

    void SaveTaskInfo(
      const int64_t&,
      const std::string&,
      const int32_t&);
    void DeleteTaskInfo(
      const int64_t&,
      const std::string&);
    std::vector<std::tuple<int64_t, std::string, int32_t>> RestoreTaskInfo();
    void SaveStoriesInfo(
      const std::string&,
      const server::story::types::Story&);
    void DeleteStoriesInfo(
      const std::string&,
      const server::story::types::Story&);
    std::unordered_map<std::string, std::vector<server::story::types::Story>> RestoreStoriesInfo();

  private:
    std::shared_ptr<service::storage::base::IStorageClient> task_storage_;
    std::shared_ptr<service::storage::base::IStorageClient> data_storage_;
    std::shared_ptr<service::storage::base::IObjectCreator> object_creator_;
};

} // service::story::storage
