#include "FollowerStorage.hpp"

#include "Logger.hpp"

namespace service::follower::storage {

FollowerStorage::FollowerStorage(
  std::shared_ptr<service::storage::base::IStorageClient> task_storage,
  std::shared_ptr<service::storage::base::IStorageClient> followers_storage,
  std::shared_ptr<service::storage::base::IStorageClient> followers_log_storage,
  std::shared_ptr<service::storage::base::IStorageClient> following_storage,
  std::shared_ptr<service::storage::base::IStorageClient> following_log_storage,
  std::shared_ptr<service::storage::base::IObjectCreator> object_creator)
  : task_storage_(task_storage),
    followers_storage_(followers_storage),
    followers_log_storage_(followers_log_storage),
    following_storage_(following_storage),
    following_log_storage_(following_log_storage),
    object_creator_(object_creator) {
  SPDLOG_INFO("FollowerStorage::Constructor");
}

void FollowerStorage::SaveTaskInfo(
  const int64_t& client_id,
  const std::string& target_username,
  const bool& followers_track,
  const bool& following_track,
  const int32_t& frequency) {
  SPDLOG_DEBUG(
    "SaveTaskInfo, client_id: {}, target_username: {}, followers: {}, following: {}, frequency: {}",
      client_id, target_username, followers_track, following_track, frequency);

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return;
  }

  if(nullptr == task_storage_) {
    SPDLOG_ERROR("Task storage is not initialized");
    return;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    storage_object.value()->AddPair(
      std::string("client-id"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::to_string(client_id)));
    storage_object.value()->AddPair(
      std::string("target-username"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, target_username));
    storage_object.value()->AddPair(
      std::string("followers-track"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::to_string(followers_track ? 1 : 0)));
    storage_object.value()->AddPair(
      std::string("following-track"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::to_string(following_track ? 1 : 0)));
    storage_object.value()->AddPair(
      std::string("frequency"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::to_string(frequency)));

    auto result = task_storage_->Create(std::move(storage_object.value()));

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to save task object to database, error: {}", static_cast<int>(result));
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

}

void FollowerStorage::DeleteTaskInfo(
  const int64_t& client_id,
  const std::string& target_username) {
  SPDLOG_DEBUG("DeleteTaskInfo, client_id: {}, target_username: {}", client_id, target_username);

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return;
  }

  if(nullptr == task_storage_) {
    SPDLOG_ERROR("Task storage is not initialized");
    return;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    storage_object.value()->AddPair(
      std::string("client-id"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::to_string(client_id)));
    storage_object.value()->AddPair(
      std::string("target-username"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, target_username));

    auto result = task_storage_->Delete(std::move(storage_object.value()));

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to delete task object from database, error: {}", static_cast<int>(result));
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

}

std::vector<std::tuple<int64_t, std::string, bool, bool, int32_t>> FollowerStorage::RestoreTaskInfo() {
  SPDLOG_DEBUG("RestoreTaskInfo");
  std::vector<std::tuple<int64_t, std::string, bool, bool, int32_t>> output;

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return output;
  }

  if(nullptr == task_storage_) {
    SPDLOG_ERROR("Task storage is not initialized");
    return output;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    std::vector<std::unique_ptr<service::storage::base::IObject>> results;
    auto result = task_storage_->Read(
      std::move(storage_object.value()),
      results);

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to read task objects from database, error: {}", static_cast<int>(result));
    } else {
      for(auto&& item : results) {
        auto values = item->GetObjectValues();

        if(std::nullopt != values) {

          std::tuple<int64_t, std::string, bool, bool, int32_t> task_info;
          try {
            for(auto& value : values.value()) {
              switch(value.second.first) {
                case service::storage::base::StorageDataTypes::eInt:

                  if(0) {
                  } else if(0 == std::strcmp("client-id", value.first.c_str())) {
                    std::get<0>(task_info) = std::stol(std::get<std::string>(value.second.second));
                  } else if(0 == std::strcmp("followers-track", value.first.c_str())) {
                    std::get<2>(task_info) = static_cast<bool>(std::stoi(std::get<std::string>(value.second.second)));
                  } else if(0 == std::strcmp("following-track", value.first.c_str())) {
                    std::get<3>(task_info) = static_cast<bool>(std::stoi(std::get<std::string>(value.second.second)));
                  } else if(0 == std::strcmp("frequency", value.first.c_str())) {
                    std::get<4>(task_info) = std::stoi(std::get<std::string>(value.second.second));
                  }

                  break;
                case service::storage::base::StorageDataTypes::eString:
                  std::get<1>(task_info) = std::get<std::string>(value.second.second);
                  break;
                default:
                  break;
              }
            }
          } catch(...) {
            SPDLOG_ERROR("Unable to cast one of parameters: {} - {}", std::get<0>(task_info), std::get<1>(task_info));
            continue;
          }
          output.push_back(task_info);

        }

      }
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

  return output;
}

void FollowerStorage::SaveFollowersInfo(
  const std::string& username,
  const std::vector<server::follower::types::Follower>& followers,
  std::shared_ptr<service::storage::base::IStorageClient> client) {
  SPDLOG_DEBUG(
    "SaveFollowersInfo, username: {}, follower.size: {}", username, followers.size());

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    storage_object.value()->AddPair(
      std::string("target-username"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, username));

    std::vector<std::unique_ptr<service::storage::base::IObject>> array;
    for(std::size_t i = 0; i < followers.size(); i++) {
      auto element = object_creator_->Create();

      if(std::nullopt != element) {
        element.value()->AddPair(
          std::string("username"),
          std::make_pair(service::storage::base::StorageDataTypes::eString, followers[i].username));
        element.value()->AddPair(
          std::string("full-name"),
          std::make_pair(service::storage::base::StorageDataTypes::eString, followers[i].full_name));
        element.value()->AddPair(
          std::string("pic-url"),
          std::make_pair(service::storage::base::StorageDataTypes::eString, followers[i].pic_url));
        element.value()->AddPair(
          std::string("time"),
          std::make_pair(service::storage::base::StorageDataTypes::eString, followers[i].timestamp.time));
        element.value()->AddPair(
          std::string("date"),
          std::make_pair(service::storage::base::StorageDataTypes::eString, followers[i].timestamp.date));
        array.push_back(std::move(element.value()));
      } else {
        SPDLOG_ERROR("Unable to create storage object for array");
      }

    }

    storage_object.value()->AddPair(
      std::string("followers-list"),
      std::make_pair(service::storage::base::StorageDataTypes::eArray, std::move(array)));

    auto result = client->Create(std::move(storage_object.value()));

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to save follower objects to database, error: {}", static_cast<int>(result));
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }
}

void FollowerStorage::DeleteFollowersInfo(
  const std::string& username,
  std::shared_ptr<service::storage::base::IStorageClient> client) {
  SPDLOG_DEBUG(
    "DeleteFollowersInfo, username: {}", username);

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    storage_object.value()->AddPair(
      std::string("target-username"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, username));

    auto result = client->Delete(std::move(storage_object.value()));

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to delete follower objects from database, error: {}", static_cast<int>(result));
    }

  } else {
    SPDLOG_ERROR("Unable to delete storage object");
  }

}

std::map<std::string, std::vector<server::follower::types::Follower>> FollowerStorage::RestoreFollowerInfo(
  std::shared_ptr<service::storage::base::IStorageClient> client) {
  SPDLOG_DEBUG("RestoreFollowerInfo");
  std::map<std::string, std::vector<server::follower::types::Follower>> output;

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    std::vector<std::unique_ptr<service::storage::base::IObject>> results;
    auto result = client->Read(
      std::move(storage_object.value()),
      results);

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to read task objects from database, error: {}", static_cast<int>(result));
    } else {
      for(auto&& item : results) {
        auto values = item->GetObjectValues();

        if(std::nullopt != values) {

          std::string target_username;
          try {
            for(auto& value : values.value()) {
              switch(value.second.first) {
                case service::storage::base::StorageDataTypes::eString:
                  target_username = std::get<std::string>(value.second.second);
                  break;
                case service::storage::base::StorageDataTypes::eArray: {
                  auto objects = std::get<std::vector<std::unique_ptr<service::storage::base::IObject>>>(std::move(value.second.second));

                  for(auto&& obj : objects) {
                    auto subvalues = obj->GetObjectValues();

                    if(std::nullopt != subvalues) {
                      server::follower::types::Follower follower_info;
                      for(auto& subvalue : subvalues.value()) {

                        if(0) {
                        } else if(0 == std::strcmp("username", subvalue.first.c_str())) {
                          follower_info.username = std::get<std::string>(subvalue.second.second);
                        } else if(0 == std::strcmp("full-name", subvalue.first.c_str())) {
                          follower_info.full_name = std::get<std::string>(subvalue.second.second);
                        } else if(0 == std::strcmp("pic-url", subvalue.first.c_str())) {
                          follower_info.pic_url = std::get<std::string>(subvalue.second.second);
                        } else if(0 == std::strcmp("time", subvalue.first.c_str())) {
                          follower_info.timestamp.time = std::get<std::string>(subvalue.second.second);
                        } else if(0 == std::strcmp("date", subvalue.first.c_str())) {
                          follower_info.timestamp.date = std::get<std::string>(subvalue.second.second);
                        }

                      }
                      output[target_username].push_back(follower_info);
                    } else {
                      SPDLOG_ERROR("Unable to parse follower info object");
                    }

                  }
                  break;
                }
                default:
                  break;
              }
            }
          } catch(...) {
            SPDLOG_ERROR("Unable to cast one of parameters: {}", target_username);
            continue;
          }

        }

      }
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

  return output;
}

void FollowerStorage::SaveFollowerLogInfo(
  const std::string& username,
  const server::follower::types::Follower& follower,
  bool type,
  std::shared_ptr<service::storage::base::IStorageClient> client) {
  SPDLOG_DEBUG(
    "SaveFollowerLogInfo, username: {}, follower.username: {}, type: {}",
      username, follower.username, type ? "followed" : "unfollowed");

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    storage_object.value()->AddPair(
      std::string("target-username"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, username));

    auto sub_object = object_creator_->Create();

    if(std::nullopt != sub_object) {
      sub_object.value()->AddPair(
        std::string("username"),
        std::make_pair(service::storage::base::StorageDataTypes::eString, follower.username));
      sub_object.value()->AddPair(
        std::string("full-name"),
        std::make_pair(service::storage::base::StorageDataTypes::eString, follower.full_name));
      sub_object.value()->AddPair(
        std::string("pic-url"),
        std::make_pair(service::storage::base::StorageDataTypes::eString, follower.pic_url));
      sub_object.value()->AddPair(
        std::string("time"),
        std::make_pair(service::storage::base::StorageDataTypes::eString, follower.timestamp.time));
      sub_object.value()->AddPair(
        std::string("date"),
        std::make_pair(service::storage::base::StorageDataTypes::eString, follower.timestamp.date));
      storage_object.value()->AddPair(
        std::string("follower"),
        std::make_pair(service::storage::base::StorageDataTypes::eObject, std::move(sub_object.value())));
    } else {
      SPDLOG_ERROR("Unable to create storage sub-object");
    }

    storage_object.value()->AddPair(
      std::string("action"),
      std::make_pair(service::storage::base::StorageDataTypes::eInt, std::to_string(type ? 1 : 0)));

    auto result = client->Create(std::move(storage_object.value()));

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to save follower log object to database, error: {}", static_cast<int>(result));
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }
}

std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>> FollowerStorage::RestoreFollowerLogInfo(
  std::shared_ptr<service::storage::base::IStorageClient> client) {
  SPDLOG_DEBUG("RestoreFollowerLogInfo");
  std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>> output;

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return output;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    std::vector<std::unique_ptr<service::storage::base::IObject>> results;
    auto result = client->Read(
      std::move(storage_object.value()),
      results);

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to read follower log objects from database, error: {}", static_cast<int>(result));
    } else {
      for(auto&& item : results) {
        auto values = item->GetObjectValues();

        if(std::nullopt != values) {

          std::string target_username;
          server::follower::types::Follower follower_info;
          bool type;
          try {
            for(auto& value : values.value()) {
              switch(value.second.first) {
                case service::storage::base::StorageDataTypes::eObject: {
                  auto subobject = std::get<std::unique_ptr<service::storage::base::IObject>>(std::move(value.second.second));
                  auto subvalues = subobject->GetObjectValues();

                  if(std::nullopt != subvalues) {
                    for(auto& subvalue : subvalues.value()) {

                      if(0) {
                      } else if(0 == std::strcmp("username", subvalue.first.c_str())) {
                        follower_info.username = std::get<std::string>(subvalue.second.second);
                      } else if(0 == std::strcmp("full-name", subvalue.first.c_str())) {
                        follower_info.full_name = std::get<std::string>(subvalue.second.second);
                      } else if(0 == std::strcmp("pic-url", subvalue.first.c_str())) {
                        follower_info.pic_url = std::get<std::string>(subvalue.second.second);
                      } else if(0 == std::strcmp("time", subvalue.first.c_str())) {
                        follower_info.timestamp.time = std::get<std::string>(subvalue.second.second);
                      } else if(0 == std::strcmp("date", subvalue.first.c_str())) {
                        follower_info.timestamp.date = std::get<std::string>(subvalue.second.second);
                      }

                    }
                  } else {
                    SPDLOG_ERROR("Unable to create storage object");
                  }

                  break;
                }
                case service::storage::base::StorageDataTypes::eString:
                  target_username = std::get<std::string>(value.second.second);
                  break;
                case service::storage::base::StorageDataTypes::eInt:
                  type = static_cast<bool>(std::stoi(std::get<std::string>(value.second.second)));
                  break;
                default:
                  break;
              }
            }
          } catch(...) {
            SPDLOG_ERROR("Unable to cast one of parameters: {}", target_username);
            continue;
          }
          output[target_username].push_back(std::make_pair(follower_info, type));

        }

      }
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

  return output;
}

void FollowerStorage::SaveFollowersInfo(
  const std::string& username,
  const std::vector<server::follower::types::Follower>& followers) {

  if(nullptr == followers_storage_) {
    SPDLOG_ERROR("Followers storage is not initialized");
    return;
  }

  SaveFollowersInfo(username, followers, followers_storage_);
}

void FollowerStorage::DeleteFollowersInfo(
  const std::string& username) {

  if(nullptr == followers_storage_) {
    SPDLOG_ERROR("Followers storage is not initialized");
    return;
  }

  DeleteFollowersInfo(username, followers_storage_);
}

std::map<std::string, std::vector<server::follower::types::Follower>> FollowerStorage::RestoreFollowerInfo() {

  if(nullptr == followers_storage_) {
    SPDLOG_ERROR("Followers storage is not initialized");
    return std::map<std::string, std::vector<server::follower::types::Follower>>();
  }

  return RestoreFollowerInfo(followers_storage_);
}

void FollowerStorage::SaveFollowerLogInfo(
  const std::string& username,
  const server::follower::types::Follower& follower,
  bool type) {

  if(nullptr == followers_log_storage_) {
    SPDLOG_ERROR("Followers log storage is not initialized");
    return;
  }

  SaveFollowerLogInfo(username, follower, type, followers_log_storage_);
}

std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>> FollowerStorage::RestoreFollowerLogInfo() {

  if(nullptr == followers_log_storage_) {
    SPDLOG_ERROR("Followers log storage is not initialized");
    return std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>>();
  }

  return RestoreFollowerLogInfo(followers_log_storage_);
}

void FollowerStorage::SaveFollowingInfo(
  const std::string& username,
  const std::vector<server::follower::types::Follower>& followers) {

  if(nullptr == following_storage_) {
    SPDLOG_ERROR("Following storage is not initialized");
    return;
  }

  SaveFollowersInfo(username, followers, following_storage_);
}

void FollowerStorage::DeleteFollowingInfo(
  const std::string& username) {

  if(nullptr == following_storage_) {
    SPDLOG_ERROR("Following storage is not initialized");
    return;
  }

  DeleteFollowersInfo(username, following_storage_);
}

std::map<std::string, std::vector<server::follower::types::Follower>> FollowerStorage::RestoreFollowingInfo() {

  if(nullptr == following_storage_) {
    SPDLOG_ERROR("Following storage is not initialized");
    return std::map<std::string, std::vector<server::follower::types::Follower>>();
  }

  return RestoreFollowerInfo(following_storage_);
}

void FollowerStorage::SaveFollowingLogInfo(
  const std::string& username,
  const server::follower::types::Follower& follower,
  bool type) {

  if(nullptr == following_log_storage_) {
    SPDLOG_ERROR("Following log storage is not initialized");
    return;
  }

  SaveFollowerLogInfo(username, follower, type, following_log_storage_);
}

std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>> FollowerStorage::RestoreFollowingLogInfo() {

  if(nullptr == following_log_storage_) {
    SPDLOG_ERROR("Following log storage is not initialized");
    return std::map<std::string, std::vector<std::pair<server::follower::types::Follower, bool>>>();
  }

  return RestoreFollowerLogInfo(following_log_storage_);
}

} // service::follower::storage
