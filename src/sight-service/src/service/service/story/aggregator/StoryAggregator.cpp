#include "StoryAggregator.hpp"
#include "TaskManager.hpp"
#include "StoryScraperClient.hpp"
#include "StoryStorage.hpp"
#include "MongoClient.hpp"
#include "MongoCreator.hpp"

#include <set>
#include <random>
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include <atomic>

namespace service::story::aggregator {

class StoryAggregator::Impl {
  public:
    explicit Impl(
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IObjectCreator>);

    server::story::types::SubscribeResponse Subscribe(
      const server::story::types::SubscribeRequest&);
    server::story::types::UnsubscribeResponse Unsubscribe(
      const server::story::types::UnsubscribeRequest&);
    server::story::types::GetAvailableStoriesResponse GetAvailableStories(
      const server::story::types::GetAvailableStoriesRequest&);
    server::story::types::GetClientTargetsResponse GetClientTargets(
      const server::story::types::GetClientTargetsRequest&);
  private:
    using StoryTaskType = std::function<void(const std::string&)>;
    using StoryManager = service::common::task::TaskManager<StoryTaskType>;
    using StorageManager = service::story::storage::StoryStorage;

    void StoryTask(
      const std::string&);

    std::string GenerateAlias(
      const int64_t&,
      const std::string&);

    void RestoreBackup();

    std::chrono::milliseconds ConvertFrequencyToMS(
      const server::story::types::PollFrequency&);

    StoryManager story_manager_;
    StorageManager storage_manager_;
    std::set<std::string> story_ids_;
    std::unordered_map<std::string, std::vector<server::story::types::Story>> stories_;
    std::mutex story_mutex_;
    std::mutex story_id_mutex_;
    std::atomic<bool> is_restoring_;
};

StoryAggregator::Impl::Impl(
  std::shared_ptr<service::storage::base::IStorageClient> task_storage,
  std::shared_ptr<service::storage::base::IStorageClient> data_storage,
  std::shared_ptr<service::storage::base::IObjectCreator> object_creator)
  : story_manager_(),
    storage_manager_(task_storage, data_storage, object_creator),
    story_ids_(),
    stories_(),
    story_mutex_(),
    story_id_mutex_(),
    is_restoring_(true) {
    RestoreBackup(); // Server is not responsive till backup restoration process is completed
}

std::chrono::milliseconds StoryAggregator::Impl::ConvertFrequencyToMS(
  const server::story::types::PollFrequency& frequency) {
  std::chrono::milliseconds ms_frequency = std::chrono::milliseconds(1);

  switch (frequency) {
    case server::story::types::PollFrequency::eRARE:
      ms_frequency = std::chrono::milliseconds(900000); // 15 minutes
      break;

    case server::story::types::PollFrequency::eMEDIUM:
      ms_frequency = std::chrono::milliseconds(300000); // 5 minutes
      break;

    case server::story::types::PollFrequency::eFAST:
      ms_frequency = std::chrono::milliseconds(60000); // 1 minute
      break;

    case server::story::types::PollFrequency::eVERY_FAST:
      ms_frequency = std::chrono::milliseconds(30000); // 30 seconds
      break;

    case server::story::types::PollFrequency::eQUANTUM:
      ms_frequency = std::chrono::milliseconds(10000); // 10 seconds
      break;

    default:
      SPDLOG_ERROR("Frequency not found: {}", static_cast<int>(frequency));
      break;
  }

  return ms_frequency;
}

void StoryAggregator::Impl::RestoreBackup() {
  is_restoring_ = true;

  stories_ = storage_manager_.RestoreStoriesInfo();

  for(auto& item : stories_) {
    for(auto& elem : item.second) {
      SPDLOG_DEBUG("Story info of {}: {}", item.first, elem.uri);
    }
  }

  auto task_info = storage_manager_.RestoreTaskInfo();
  for(auto& item : task_info) {
    server::story::types::SubscribeRequest request {
      .client = std::get<0>(item),
      .target = server::story::types::Target {
        .username = std::get<1>(item),
        .frequency = static_cast<server::story::types::PollFrequency>(std::get<2>(item))
      }
    };

    this->Subscribe(request);
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // TODO: Fix crash without delay
  }

  is_restoring_ = false;
}

void StoryAggregator::Impl::StoryTask(
  const std::string& username) {
  SPDLOG_INFO("Story task for {}", username);
  const std::size_t kRetryThreshold = 5;
  namespace sss = service::story::scraper;

  std::vector<sss::StoryInfo> stories_info;
  std::size_t retries = 0;
  while(true) {
    sss::Error error;

    try {
      stories_info = sss::get_stories(username, error);
    } catch(...) {
      SPDLOG_ERROR("Exception from get_stories");
      error = sss::Error::eNOK;
    }

    if(sss::Error::eOK != error) {
      SPDLOG_ERROR("Failed to get_stories, retry #{} in 10s", retries++);
      std::this_thread::sleep_for(std::chrono::seconds(10));
    } else {
      break;
    }

    if(kRetryThreshold < retries) {
      SPDLOG_ERROR("Failed to get_stories {} after {} retries, aborting", username, retries);
      return;
    }

  }

  std::unique_lock<std::mutex> id_lock(story_id_mutex_);
  std::vector<std::string> stories_to_download;
  for(const auto& story : stories_info) {
    auto insert = story_ids_.emplace(story.id);

    if(true == insert.second) {
      stories_to_download.push_back(story.id);
    }

  }
  id_lock.unlock();

  if(true == stories_to_download.empty()) {
    SPDLOG_DEBUG("No stories to download for {}", username);
    return;
  }

  std::vector<sss::StoryData> stories_data;
  retries = 0;
  while(kRetryThreshold > retries) {
    sss::Error dl_error;

    try {
      stories_data = sss::download_stories(username, stories_to_download, dl_error);
    } catch(...) {
      SPDLOG_ERROR("Exception from download_stories");
      dl_error = sss::Error::eNOK;
    }

    if(sss::Error::eOK != dl_error) {
      SPDLOG_ERROR("Failed to download_stories, retry #{} in 10s", retries++);
      std::this_thread::sleep_for(std::chrono::seconds(10));
    } else {
      break;
    }

    if(kRetryThreshold < retries) {
      SPDLOG_ERROR("Failed to download_stories {} after {} retries, aborting", username, retries);
      id_lock.lock();
      for(const auto& id : stories_to_download) {
        story_ids_.erase(id); // Clean-up unsuccessful
      }
      id_lock.unlock();
      return;
    }

  }

  std::lock_guard<std::mutex> story_lock(story_mutex_);
  for(const auto& data : stories_data) {
    server::story::types::Timestamp timestamp {
      .date = "01.01.2020",
      .time = "20:00"
    };

    for(const auto& ts_data : stories_info) {

      if(ts_data.id == data.id) {
        timestamp.date = ts_data.date;
        timestamp.time = ts_data.time;
      }

    }

    server::story::types::Story story {
      .timestamp = timestamp,
      .uri = data.uri
    };

    stories_[username].push_back(story);
    storage_manager_.SaveStoriesInfo(username, story);
  }

  SPDLOG_DEBUG("Downloaded {} stories from {}", stories_data.size(), username);
}

std::string StoryAggregator::Impl::GenerateAlias(
  const int64_t& client_id,
  const std::string& username) {
  return std::to_string(client_id) + std::string("_") + username;
}

server::story::types::SubscribeResponse StoryAggregator::Impl::Subscribe(
  const server::story::types::SubscribeRequest& request) {
  SPDLOG_INFO("Subscribe request, client: {}, username: {}, frequency: {}", request.client, request.target.username,
    static_cast<int>(request.target.frequency));
  StoryManager::Error error = StoryManager::Error::eNOK;
  const auto kDelay = ConvertFrequencyToMS(request.target.frequency);

  if(kDelay > std::chrono::milliseconds(1)) {
    auto alias = GenerateAlias(request.client, request.target.username);
    error = story_manager_.RunTask(
      alias,
      kDelay,
      std::bind(&StoryAggregator::Impl::StoryTask, this, std::placeholders::_1),
      std::string(request.target.username)); // TODO: fix a bug with arguments lifecycle
  }

  server::story::types::SubscribeResponse response;

  if(StoryManager::Error::eOK == error) {

    if(false == is_restoring_) {
      storage_manager_.SaveTaskInfo(
        request.client,
        request.target.username,
        static_cast<int>(request.target.frequency));
    }

    response.error = 0;
  } else {
    response.error = -2;
  }

  return response;
}

server::story::types::UnsubscribeResponse StoryAggregator::Impl::Unsubscribe(
  const server::story::types::UnsubscribeRequest& request) {
  SPDLOG_INFO("Unsubscribe request, client: {}, username: {}", request.client, request.username);

  auto alias = GenerateAlias(request.client, request.username);
  auto error = story_manager_.StopTask(alias);

  server::story::types::UnsubscribeResponse response;

  if(StoryManager::Error::eOK == error) {
    storage_manager_.DeleteTaskInfo(request.client, request.username);
    response.error = 0;
  } else {
    response.error = -2;
  }

  return response;
}

server::story::types::GetAvailableStoriesResponse StoryAggregator::Impl::GetAvailableStories(
  const server::story::types::GetAvailableStoriesRequest& request) {
  SPDLOG_INFO("Get stories request, client: {}, username: {}", request.client, request.username);
  server::story::types::GetAvailableStoriesResponse response;

  std::unique_lock<std::mutex> lock(story_mutex_);
  auto stories = stories_.find(request.username);

  if(stories_.end() != stories) {
    response.stories = stories->second;
    response.error = 0;
  } else {
    response.error = -2;
  }

  lock.unlock();
  return response;
}

server::story::types::GetClientTargetsResponse StoryAggregator::Impl::GetClientTargets(
  const server::story::types::GetClientTargetsRequest& request) {
  SPDLOG_INFO("Get client targets request, client: {}", request.client);
  server::story::types::GetClientTargetsResponse response;

  auto task_info = storage_manager_.RestoreTaskInfo();
  for(auto& item : task_info) {
    try{

      if(std::get<0>(item) == request.client) {
        auto target = server::story::types::Target {
          .username = std::get<1>(item),
          .frequency = static_cast<server::story::types::PollFrequency>(std::get<2>(item))
        };

        response.targets.push_back(target);
      }

    } catch(...) {
      SPDLOG_WARN("GetClientTargets request, exception during parsing one of client #{} targets", request.client);
    }
  }

  response.error = 0;
  return response;
}

StoryAggregator::~StoryAggregator() = default;

StoryAggregator::StoryAggregator()
  : impl_(std::make_unique<Impl>(
      std::make_shared<service::storage::mongo::MongoClient>(
        std::string("mongodb://localhost:27017"),
        std::string("story-service-test-db"),
        std::string("story-service-task-collection")),
      std::make_shared<service::storage::mongo::MongoClient>(
        std::string("mongodb://localhost:27017"),
        std::string("story-service-test-db"),
        std::string("story-service-data-collection")),
      std::make_shared<service::storage::mongo::MongoObjectCreator>())) {
}

server::story::types::SubscribeResponse StoryAggregator::Subscribe(
  const server::story::types::SubscribeRequest& request) {
  server::story::types::SubscribeResponse response;

  if(nullptr != impl_) {
    response = impl_->Subscribe(request);
  } else {
    response.error = -2;
  }

  return response;
}

server::story::types::UnsubscribeResponse StoryAggregator::Unsubscribe(
  const server::story::types::UnsubscribeRequest& request) {
  server::story::types::UnsubscribeResponse response;

  if(nullptr != impl_) {
    response = impl_->Unsubscribe(request);
  } else {
    response.error = -2;
  }

  return response;
}

server::story::types::GetAvailableStoriesResponse StoryAggregator::GetAvailableStories(
  const server::story::types::GetAvailableStoriesRequest& request) {
  server::story::types::GetAvailableStoriesResponse response;

  if(nullptr != impl_) {
    response = impl_->GetAvailableStories(request);
  } else {
    response.error = -2;
  }

  return response;
}

server::story::types::GetClientTargetsResponse StoryAggregator::GetClientTargets(
  const server::story::types::GetClientTargetsRequest& request) {
  server::story::types::GetClientTargetsResponse response;

  if(nullptr != impl_) {
    response = impl_->GetClientTargets(request);
  } else {
    response.error = -2;
  }

  return response;
}

} // service::story::aggregator
