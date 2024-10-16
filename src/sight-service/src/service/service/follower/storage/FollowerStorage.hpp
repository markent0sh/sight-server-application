#pragma once

#include <map>
#include <tuple>

#include "FollowerServer.hpp"
#include "StorageClient.hpp"
#include "StorageCreator.hpp"

namespace service::follower::storage {

class FollowerStorage {
  public:
    FollowerStorage(
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IObjectCreator>);
    ~FollowerStorage() = default;
    FollowerStorage(FollowerStorage&&) = default;
    FollowerStorage& operator=(FollowerStorage&&) = default;
    FollowerStorage(const FollowerStorage&) = delete;
    FollowerStorage& operator=(const FollowerStorage&) = delete;

    void SaveTaskInfo(
      const int64_t&,
      const std::string&,
      const bool&,
      const bool&,
      const int32_t&);
    void DeleteTaskInfo(
      const int64_t&,
      const std::string&);
    std::vector<std::tuple<int64_t, std::string, bool, bool, int32_t>> RestoreTaskInfo();

    void SaveFollowersInfo(
      const std::string&,
      const std::vector<server::follower::types::Follower>&);
    void DeleteFollowersInfo(
      const std::string&);
    std::map<std::string, std::vector<server::follower::types::Follower>> RestoreFollowerInfo();

    void SaveFollowerLogInfo(
      const std::string&,
      const server::follower::types::Follower&,
      bool);
    std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>> RestoreFollowerLogInfo();

    void SaveFollowingInfo(
      const std::string&,
      const std::vector<server::follower::types::Follower>&);
    void DeleteFollowingInfo(
      const std::string&);
    std::map<std::string, std::vector<server::follower::types::Follower>> RestoreFollowingInfo();

    void SaveFollowingLogInfo(
      const std::string&,
      const server::follower::types::Follower&,
      bool);
    std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>> RestoreFollowingLogInfo();

  private:
    void SaveFollowersInfo(
      const std::string&,
      const std::vector<server::follower::types::Follower>&,
      std::shared_ptr<service::storage::base::IStorageClient>);
    void DeleteFollowersInfo(
      const std::string&,
      std::shared_ptr<service::storage::base::IStorageClient>);
    std::map<std::string, std::vector<server::follower::types::Follower>> RestoreFollowerInfo(
      std::shared_ptr<service::storage::base::IStorageClient>);

    void SaveFollowerLogInfo(
      const std::string&,
      const server::follower::types::Follower&,
      bool,
      std::shared_ptr<service::storage::base::IStorageClient>);
    std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>> RestoreFollowerLogInfo(
      std::shared_ptr<service::storage::base::IStorageClient>);

    std::shared_ptr<service::storage::base::IStorageClient> task_storage_;
    std::shared_ptr<service::storage::base::IStorageClient> followers_storage_;
    std::shared_ptr<service::storage::base::IStorageClient> followers_log_storage_;
    std::shared_ptr<service::storage::base::IStorageClient> following_storage_;
    std::shared_ptr<service::storage::base::IStorageClient> following_log_storage_;
    std::shared_ptr<service::storage::base::IObjectCreator> object_creator_;
};

} // service::follower::storage
