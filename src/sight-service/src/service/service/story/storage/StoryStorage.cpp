#include "StoryStorage.hpp"

#include "Logger.hpp"

namespace service::story::storage {

StoryStorage::StoryStorage(
  std::shared_ptr<service::storage::base::IStorageClient> task_storage,
  std::shared_ptr<service::storage::base::IStorageClient> data_storage,
  std::shared_ptr<service::storage::base::IObjectCreator> object_creator)
  : task_storage_(task_storage),
    data_storage_(data_storage),
    object_creator_(object_creator) {
  SPDLOG_INFO("StoryStorage::Constructor");
}

void StoryStorage::SaveTaskInfo(
  const int64_t& client_id,
  const std::string& target_username,
  const int32_t& frequency) {
  SPDLOG_DEBUG("SaveTaskInfo, client_id: {}, target_username: {}", client_id, target_username);

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

void StoryStorage::DeleteTaskInfo(
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

std::vector<std::tuple<int64_t, std::string, int32_t>> StoryStorage::RestoreTaskInfo() {
  SPDLOG_DEBUG("RestoreTaskInfo");
  std::vector<std::tuple<int64_t, std::string, int32_t>> output;

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

          std::tuple<int64_t, std::string, int32_t> task_info;
          try {
            for(auto& value : values.value()) {
              switch(value.second.first) {
                case service::storage::base::StorageDataTypes::eInt:

                  if(0) {
                  } else if(0 == std::strcmp("client-id", value.first.c_str())) {
                    std::get<0>(task_info) = std::stol(std::get<std::string>(value.second.second));
                  } else if(0 == std::strcmp("frequency", value.first.c_str())) {
                    std::get<2>(task_info) = std::stoi(std::get<std::string>(value.second.second));
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

void StoryStorage::SaveStoriesInfo(
  const std::string& username,
  const server::story::types::Story& story) {
  SPDLOG_DEBUG("SaveTaskInfo, username: {}, story uri: {}", username, story.uri);

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return;
  }

  if(nullptr == data_storage_) {
    SPDLOG_ERROR("Data storage is not initialized");
    return;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    storage_object.value()->AddPair(
      std::string("target-username"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, username));
    storage_object.value()->AddPair(
      std::string("uri"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, story.uri));
    storage_object.value()->AddPair(
      std::string("date"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, story.timestamp.date));
    storage_object.value()->AddPair(
      std::string("time"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, story.timestamp.time));

    auto result = data_storage_->Create(std::move(storage_object.value()));

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to save data object to database, error: {}", static_cast<int>(result));
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

}

void StoryStorage::DeleteStoriesInfo(
  const std::string& username,
  const server::story::types::Story& story) {
  SPDLOG_DEBUG("DeleteStoriesInfo, username: {}, story uri: {}", username, story.uri);

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return;
  }

  if(nullptr == data_storage_) {
    SPDLOG_ERROR("Data storage is not initialized");
    return;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    storage_object.value()->AddPair(
      std::string("target-username"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, username));
    storage_object.value()->AddPair(
      std::string("uri"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, story.uri));
    storage_object.value()->AddPair(
      std::string("date"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, story.timestamp.date));
    storage_object.value()->AddPair(
      std::string("time"),
      std::make_pair(service::storage::base::StorageDataTypes::eString, story.timestamp.time));

    auto result = data_storage_->Delete(std::move(storage_object.value()));

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to delete data object from database, error: {}", static_cast<int>(result));
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

}

std::unordered_map<std::string, std::vector<server::story::types::Story>> StoryStorage::RestoreStoriesInfo() {
  SPDLOG_DEBUG("RestoreStoriesInfo");
  std::unordered_map<std::string, std::vector<server::story::types::Story>> output;

  if(nullptr == object_creator_) {
    SPDLOG_ERROR("Storage object creator is not initialized");
    return output;
  }

  if(nullptr == data_storage_) {
    SPDLOG_ERROR("Data storage is not initialized");
    return output;
  }

  auto storage_object = object_creator_->Create();

  if(std::nullopt != storage_object) {
    std::vector<std::unique_ptr<service::storage::base::IObject>> results;
    auto result = data_storage_->Read(
      std::move(storage_object.value()),
      results);

    if(service::storage::base::StorageError::eOk != result) {
      SPDLOG_ERROR("Unable to read data objects from database, error: {}", static_cast<int>(result));
    } else {
      for(auto&& item : results) {
        auto values = item->GetObjectValues();

        if(std::nullopt != values) {

          std::string username;
          server::story::types::Story story_info;
          try {
            for(auto& value : values.value()) {
              switch(value.second.first) {
                case service::storage::base::StorageDataTypes::eString:

                  if(0) {
                  } else if(0 == std::strcmp("target-username", value.first.c_str())) {
                    username = std::get<std::string>(value.second.second);
                  } else if(0 == std::strcmp("uri", value.first.c_str())) {
                    story_info.uri = std::get<std::string>(value.second.second);
                  } else if(0 == std::strcmp("date", value.first.c_str())) {
                    story_info.timestamp.date = std::get<std::string>(value.second.second);
                  } else if(0 == std::strcmp("time", value.first.c_str())) {
                    story_info.timestamp.time = std::get<std::string>(value.second.second);
                  }

                  break;
                default:
                  break;
              }
            }
          } catch(...) {
            SPDLOG_ERROR("Unable to parse backup for {}", username);
            continue;
          }
          output[username].push_back(story_info);

        }

      }
    }

  } else {
    SPDLOG_ERROR("Unable to create storage object");
  }

  return output;
}

} // service::story::storage
