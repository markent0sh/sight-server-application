#include "FollowerAggregator.hpp"
#include "TaskManager.hpp"
#include "FollowerScraperClient.hpp"
#include "Time.hpp"
#include "FollowerStorage.hpp"
#include "MongoClient.hpp"
#include "MongoCreator.hpp"

#include <map>
#include <mutex>
#include <random>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <functional>
#include <atomic>

namespace service::follower::aggregator {

class FollowerAggregator::Impl {
  public:
    explicit Impl(
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IStorageClient>,
      std::shared_ptr<service::storage::base::IObjectCreator>);

    server::follower::types::SubscribeResponse Subscribe(
      const server::follower::types::SubscribeRequest&);
    server::follower::types::UnsubscribeResponse Unsubscribe(
      const server::follower::types::UnsubscribeRequest&);
    server::follower::types::GetFollowersResponse GetFollowers(
      const server::follower::types::GetFollowersRequest&);
    server::follower::types::GetFollowingResponse GetFollowing(
      const server::follower::types::GetFollowingRequest&);
    server::follower::types::GetClientTargetsResponse GetClientTargets(
      const server::follower::types::GetClientTargetsRequest&);
  private:
    enum class Action {
      kNone = 0,
      kFollowed = 1,
      kUnfollowed = 2
    };

    struct UserLog {
      server::follower::types::Follower user;
      Action action;
    };

    using FollowersTaskType = std::function<void(const std::string&, const std::pair<bool, bool>&)>;
    using FollowerManager = service::common::task::TaskManager<FollowersTaskType>;
    using StorageManager = service::follower::storage::FollowerStorage;

    void FollowersTask(
      const std::string&,
      const std::pair<bool, bool>&);

    std::string GenerateAlias(
      const int64_t&,
      const std::string&);

    void RestoreBackup();

    std::chrono::milliseconds ConvertFrequencyToMS(const server::follower::types::PollFrequency&);

    FollowerManager follower_manager_;
    StorageManager storage_manager_;

    using Username = std::string;
    using AssociateList = std::vector<server::follower::types::Follower>;
    using ChangeLog = std::vector<UserLog>;

    std::map<Username, AssociateList> followers_;
    std::map<Username, AssociateList> following_;
    std::map<Username, ChangeLog> followers_change_logs_;
    std::map<Username, ChangeLog> following_change_logs_;

    std::mutex followers_mutex_;
    std::mutex following_mutex_;
    std::mutex followers_log_mutex_;
    std::mutex following_log_mutex_;
    std::atomic<bool> is_restoring_;
};

FollowerAggregator::Impl::Impl(
  std::shared_ptr<service::storage::base::IStorageClient> task_storage,
  std::shared_ptr<service::storage::base::IStorageClient> followers_storage,
  std::shared_ptr<service::storage::base::IStorageClient> followers_log_storage,
  std::shared_ptr<service::storage::base::IStorageClient> following_storage,
  std::shared_ptr<service::storage::base::IStorageClient> following_log_storage,
  std::shared_ptr<service::storage::base::IObjectCreator> object_creator)
  : follower_manager_(),
    storage_manager_(
      task_storage,
      followers_storage,
      followers_log_storage,
      following_storage,
      following_log_storage,
      object_creator),
    followers_(),
    following_(),
    followers_change_logs_(),
    following_change_logs_(),
    followers_mutex_(),
    following_mutex_(),
    followers_log_mutex_(),
    following_log_mutex_(),
    is_restoring_(true) {
    RestoreBackup(); // Server is not responsive till backup restoration process is completed
}

std::chrono::milliseconds FollowerAggregator::Impl::ConvertFrequencyToMS(
  const server::follower::types::PollFrequency& frequency) {
  std::chrono::milliseconds ms_frequency = std::chrono::milliseconds(1);

  switch (frequency) {
    case server::follower::types::PollFrequency::eRARE:
      ms_frequency = std::chrono::milliseconds(7200000); // 2 hours
      break;

    case server::follower::types::PollFrequency::eMEDIUM:
      ms_frequency = std::chrono::milliseconds(3600000); // 1 hour
      break;

    case server::follower::types::PollFrequency::eFAST:
      ms_frequency = std::chrono::milliseconds(1800000); // 30 minutes
      break;

    case server::follower::types::PollFrequency::eVERY_FAST:
      ms_frequency = std::chrono::milliseconds(900000); // 15 minutes
      break;

    case server::follower::types::PollFrequency::eQUANTUM:
      ms_frequency = std::chrono::milliseconds(300000); // 5 minutes
      break;

    default:
      SPDLOG_ERROR("Frequency not found: {}", static_cast<int>(frequency));
      break;
  }

  return ms_frequency;
}

void FollowerAggregator::Impl::RestoreBackup() {
  is_restoring_ = true;

  followers_ = storage_manager_.RestoreFollowerInfo();
  SPDLOG_INFO("Restored follower lists, target count: {}", followers_.size());

  for(auto& item : followers_) {
    SPDLOG_DEBUG("Target username: {}, followers list size: {}", item.first, item.second.size());
  }

  auto follower_log = storage_manager_.RestoreFollowerLogInfo();

  for(auto& item : follower_log) {
    followers_change_logs_[item.first] = ChangeLog(item.second.size());
    std::transform(item.second.begin(), item.second.end(), followers_change_logs_[item.first].begin(), [&](auto& subitem) {
      return UserLog {
        .user = subitem.first,
        .action = subitem.second ? Action::kFollowed : Action::kUnfollowed
      };
    });
  }

  SPDLOG_INFO("Restored follower log lists, target count: {}", followers_change_logs_.size());

  for(auto& item : followers_change_logs_) {
    SPDLOG_DEBUG("Target username: {}, followers log list size: {}", item.first, item.second.size());
  }

  following_ = storage_manager_.RestoreFollowingInfo();
  SPDLOG_INFO("Restored following lists, target count: {}", following_.size());

  for(auto& item : following_) {
    SPDLOG_DEBUG("Target username: {}, following list size: {}", item.first, item.second.size());
  }

  auto following_log = storage_manager_.RestoreFollowingLogInfo();

  for(auto& item : following_log) {
    following_change_logs_[item.first] = ChangeLog(item.second.size());
    std::transform(item.second.begin(), item.second.end(), following_change_logs_[item.first].begin(), [&](auto& subitem) {
      return UserLog {
        .user = subitem.first,
        .action = subitem.second ? Action::kFollowed : Action::kUnfollowed
      };
    });
  }

  SPDLOG_INFO("Restored following log lists, target count: {}", following_change_logs_.size());

  for(auto& item : following_change_logs_) {
    SPDLOG_DEBUG("Target username: {}, following log list size: {}", item.first, item.second.size());
  }

  auto task_info = storage_manager_.RestoreTaskInfo();
  for(auto& item : task_info) {
    server::follower::types::SubscribeRequest request {
      .client = std::get<0>(item),
      .target = server::follower::types::Target {
        .username = std::get<1>(item),
        .followers = std::get<2>(item),
        .following = std::get<3>(item),
        .frequency = static_cast<server::follower::types::PollFrequency>(std::get<4>(item))
      }
    };

    this->Subscribe(request);
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // TODO: Fix crash without delay
  }

  is_restoring_ = false;
}

void FollowerAggregator::Impl::FollowersTask(
  const std::string& username,
  const std::pair<bool, bool>& targets) {
  SPDLOG_INFO(
    "Task for {}, followers track: {}, following track: {}",
      username, targets.first ? "true" : "false", targets.second ? "true" : "false");
  const std::size_t kRetryThreshold = 5;
  namespace sss = service::follower::scraper;
  namespace sct = service::common::time;

  if(targets.first) { // Followers tracking
    std::vector<sss::UserInfo> scraper_followers_list;
    std::size_t retries = 0;
    while(true) {
      sss::Error fers_error;

      try {
        scraper_followers_list = sss::get_target_followers(username, fers_error);
      } catch(...) {
        SPDLOG_ERROR("Exception from get_target_followers");
        fers_error = sss::Error::eNOK;
      }

      if(sss::Error::eOK != fers_error) {
        SPDLOG_ERROR("Failed to get_target_followers, retry #{} in 10s", retries++);
        std::this_thread::sleep_for(std::chrono::seconds(10));
      } else {
        break;
      }

      if(kRetryThreshold < retries) {
        SPDLOG_ERROR("Failed to get_target_followers {} after {} retries, aborting", username, retries);
        return;
      }

    }

    auto current_time = sct::get_current_utc_time();
    server::follower::types::Timestamp timestamp {
      .date = current_time.first,
      .time = current_time.second
    };
    SPDLOG_DEBUG("Followers timestamp, time: {}, date: {}", timestamp.time, timestamp.date);

    AssociateList followers_list(scraper_followers_list.size());
    std::transform(scraper_followers_list.begin(), scraper_followers_list.end(), followers_list.begin(),
      [&](auto& item) {
        return server::follower::types::Follower {
          .username = item.username,
          .full_name = item.full_name,
          .pic_url = item.pic_url,
          .timestamp = timestamp
        };
      }
    );

    std::lock_guard<std::mutex> followers_lock(followers_mutex_);
    std::lock_guard<std::mutex> followers_log_lock(followers_log_mutex_);
    auto current_list = followers_.find(username);

    if(followers_.end() != current_list) {
      AssociateList unfollowed;
      std::for_each(current_list->second.begin(), current_list->second.end(), [&](auto& item) {

        if(followers_list.end() == std::find(followers_list.begin(), followers_list.end(), item)) {
          item.timestamp = timestamp;
          unfollowed.push_back(item);
          followers_change_logs_[username].push_back(UserLog {
            .user = item,
            .action = Action::kUnfollowed
          });
          storage_manager_.SaveFollowerLogInfo(username, item, false);
          SPDLOG_DEBUG("Followers :: User {} unfollowed target {}",item.username, username);
        }

      });

      std::for_each(unfollowed.begin(), unfollowed.end(), [&](auto& item) {
        current_list->second.erase(std::find(current_list->second.begin(), current_list->second.end(), item));
      });

      SPDLOG_DEBUG("Followers :: {} users unfollowed {} target, current list size {}", unfollowed.size(), username, current_list->second.size());

      std::for_each(followers_list.begin(), followers_list.end(), [&](auto& item) {

        if(current_list->second.end() == std::find(current_list->second.begin(), current_list->second.end(), item)) {
          item.timestamp = timestamp;
          current_list->second.push_back(item);
          followers_change_logs_[username].push_back(UserLog {
            .user = item,
            .action = Action::kFollowed
          });
          storage_manager_.SaveFollowerLogInfo(username, item, true);
          SPDLOG_DEBUG("Followers :: User {} followed target {}", item.username, username);
        }

      });
    } else {
      followers_[username] = followers_list;
      SPDLOG_DEBUG("Followers :: Inited target {} followed list with {} users", username, followers_[username].size());
    }

    storage_manager_.DeleteFollowersInfo(username);
    storage_manager_.SaveFollowersInfo(username, followers_list);
  }

  if(targets.second) { // Following tracking
    std::vector<sss::UserInfo> scraper_following_list;
    std::size_t retries = 0;
    while(true) {
      sss::Error fing_error;

      try {
        scraper_following_list = sss::get_target_following(username, fing_error);
      } catch(...) {
        SPDLOG_ERROR("Exception from get_target_following");
        fing_error = sss::Error::eNOK;
      }

      if(sss::Error::eOK != fing_error) {
        SPDLOG_ERROR("Failed to get_target_following, retry #{} in 10s", retries++);
        std::this_thread::sleep_for(std::chrono::seconds(10));
      } else {
        break;
      }

      if(kRetryThreshold < retries) {
        SPDLOG_ERROR("Failed to get_target_following {} after {} retries, aborting", username, retries);
        return;
      }

    }

    auto current_time = sct::get_current_utc_time();
    server::follower::types::Timestamp timestamp {
      .date = current_time.first,
      .time = current_time.second
    };
    SPDLOG_DEBUG("Following timestamp, time: {}, date: {}", timestamp.time, timestamp.date);

    AssociateList following_list(scraper_following_list.size());
    std::transform(scraper_following_list.begin(), scraper_following_list.end(), following_list.begin(),
      [&](auto& item) {
        return server::follower::types::Follower {
          .username = item.username,
          .full_name = item.full_name,
          .pic_url = item.pic_url,
          .timestamp = timestamp
        };
      }
    );

    std::lock_guard<std::mutex> following_lock(following_mutex_);
    std::lock_guard<std::mutex> following_log_lock(following_log_mutex_);
    auto current_list = following_.find(username);

    if(following_.end() != current_list) {
      AssociateList unfollowed;
      std::for_each(current_list->second.begin(), current_list->second.end(), [&](auto& item) {

        if(following_list.end() == std::find(following_list.begin(), following_list.end(), item)) {
          item.timestamp = timestamp;
          unfollowed.push_back(item);
          following_change_logs_[username].push_back(UserLog {
            .user = item,
            .action = Action::kUnfollowed
          });
          storage_manager_.SaveFollowingLogInfo(username, item, false);
          SPDLOG_DEBUG("Following :: Target {} unfollowed user {}", username, item.username);
        }

      });

      std::for_each(unfollowed.begin(), unfollowed.end(), [&](auto& item) {
        current_list->second.erase(std::find(current_list->second.begin(), current_list->second.end(), item));
      });

      SPDLOG_DEBUG("Following :: Target {} unfollowed {} users, current list size {}", username, unfollowed.size(), current_list->second.size());

      std::for_each(following_list.begin(), following_list.end(), [&](auto& item) {

        if(current_list->second.end() == std::find(current_list->second.begin(), current_list->second.end(), item)) {
          item.timestamp = timestamp;
          current_list->second.push_back(item);
          following_change_logs_[username].push_back(UserLog {
            .user = item,
            .action = Action::kFollowed
          });
          storage_manager_.SaveFollowingLogInfo(username, item, true);
          SPDLOG_DEBUG("Following :: Target {} followed user {}", username, item.username);
        }

      });
    } else {
      following_[username] = following_list;
      SPDLOG_DEBUG("Following :: Inited target {} following list with {} users", username, following_[username].size());
    }

    storage_manager_.DeleteFollowingInfo(username);
    storage_manager_.SaveFollowingInfo(username, following_list);
  }

}

std::string FollowerAggregator::Impl::GenerateAlias(
  const int64_t& client_id,
  const std::string& username) {
  return std::to_string(client_id) + "_" + username;
}

server::follower::types::SubscribeResponse FollowerAggregator::Impl::Subscribe(
  const server::follower::types::SubscribeRequest& request) {
  SPDLOG_INFO(
    "Subscribe request, client: {}, username: {}, followers: {}, following: {}, frequency: {}",
      request.client, request.target.username, request.target.followers, request.target.following,
        static_cast<int>(request.target.frequency));
  FollowerManager::Error error = FollowerManager::Error::eNOK;
  const auto kDelay = ConvertFrequencyToMS(request.target.frequency);

  if(kDelay > std::chrono::milliseconds(1)) {
    auto alias = GenerateAlias(request.client, request.target.username);
    error = follower_manager_.RunTask(
      alias,
      kDelay,
      std::bind(&FollowerAggregator::Impl::FollowersTask, this, std::placeholders::_1 , std::placeholders::_2),
      std::string(request.target.username),
      std::make_pair(request.target.followers, request.target.following));
  }

  server::follower::types::SubscribeResponse response;

  if(FollowerManager::Error::eOK == error) {

    if(false == is_restoring_) {
      storage_manager_.SaveTaskInfo(
        request.client,
        request.target.username,
        request.target.followers,
        request.target.following,
        static_cast<int>(request.target.frequency));
    }

    response.error = 0;
  } else {
    response.error = -2;
  }

  return response;
}

server::follower::types::UnsubscribeResponse FollowerAggregator::Impl::Unsubscribe(
  const server::follower::types::UnsubscribeRequest& request) {
  SPDLOG_INFO("Unsubscribe request, client: {}, username: {}", request.client, request.username);
  auto alias = GenerateAlias(request.client, request.username);
  auto error = follower_manager_.StopTask(alias);

  server::follower::types::UnsubscribeResponse response;

  if(FollowerManager::Error::eOK == error) {
    storage_manager_.DeleteTaskInfo(request.client, request.username);
    response.error = 0;
  } else {
    response.error = -2;
  }

  return response;
}

server::follower::types::GetFollowersResponse FollowerAggregator::Impl::GetFollowers(
  const server::follower::types::GetFollowersRequest& request) {
  SPDLOG_INFO(
    "GetFollowers request, client: {}, username: {}, from: [date: {}, time: {}], to: [date: {}, time: {}]",
      request.client, request.username, request.from.date, request.from.time, request.to.date, request.to.time);
  server::follower::types::GetFollowersResponse response;
  response.error = 0;

  std::unique_lock<std::mutex> followers_lock(followers_mutex_);
  auto followers = followers_.find(request.username);

  if(followers_.end() != followers) {
    response.all = followers->second;
  } else {
    SPDLOG_WARN("GetFollowers request, not found any followers data for username {}", request.username);
    response.error = -2;
    return response;
  }

  followers_lock.unlock();

  std::unique_lock<std::mutex> log_lock(followers_log_mutex_);
  auto change_log = followers_change_logs_.find(request.username);

  if(followers_change_logs_.end() != change_log) {
    auto list = change_log->second;
    log_lock.unlock();

    decltype(list) ranged;
    std::copy_if(list.begin(), list.end(), std::back_inserter(ranged), [&](auto& log_event) {
      try {
        using TimestampLocal = service::common::time::Timestamp;
        auto timestamp = TimestampLocal(log_event.user.timestamp.date, log_event.user.timestamp.time);

        return timestamp >= TimestampLocal(request.from.date, request.from.time) &&
          timestamp <= TimestampLocal(request.to.date, request.to.time);
      } catch(const std::invalid_argument& exception) {
        SPDLOG_ERROR("GetFollowers request, incorrent date or time format, error: {}", exception.what());
        return false;
      } catch(...) {
        SPDLOG_ERROR("GetFollowers request catch unknown error");
        return false;
      }
    });

    response.subscribed.clear();
    for(const auto& log_event : ranged) {

      if(Action::kFollowed == log_event.action) {
        response.subscribed.push_back(log_event.user);
      }

    }

    response.unsubscribed.clear();
    for(const auto& log_event : ranged) {

      if(Action::kUnfollowed == log_event.action) {
        response.unsubscribed.push_back(log_event.user);
      }

    }

  } else {
    SPDLOG_WARN("GetFollowers request, not found any followers data for username {}", request.username);
    response.error = -2;
  }

  return response;
}

server::follower::types::GetFollowingResponse FollowerAggregator::Impl::GetFollowing(
  const server::follower::types::GetFollowingRequest& request) {
  SPDLOG_INFO(
    "GetFollowing request, client: {}, username: {}, from: [date: {}, time: {}], to: [date: {}, time: {}]",
      request.client, request.username, request.from.date, request.from.time, request.to.date, request.to.time);
  server::follower::types::GetFollowingResponse response;
  response.error = 0;

  std::unique_lock<std::mutex> following_lock(following_mutex_);
  auto following = following_.find(request.username);

  if(following_.end() != following) {
    response.all = following->second;
  } else {
    SPDLOG_WARN("GetFollowing request, not found any following data for username {}", request.username);
    response.error = -2;
    return response;
  }

  following_lock.unlock();

  std::unique_lock<std::mutex> log_lock(following_log_mutex_);
  auto change_log = following_change_logs_.find(request.username);

  if(following_change_logs_.end() != change_log) {
    auto list = change_log->second;
    log_lock.unlock();

    decltype(list) ranged;
    std::copy_if(list.begin(), list.end(), std::back_inserter(ranged), [&](auto& log_event) {
      try {
        using TimestampLocal = service::common::time::Timestamp;
        auto timestamp = TimestampLocal(log_event.user.timestamp.date, log_event.user.timestamp.time);

        return timestamp >= TimestampLocal(request.from.date, request.from.time) &&
          timestamp <= TimestampLocal(request.to.date, request.to.time);
      } catch(const std::invalid_argument& exception) {
        SPDLOG_ERROR("GetFollowing request, incorrent date or time format, error: {}", exception.what());
        return false;
      } catch(...) {
        SPDLOG_ERROR("GetFollowing request catch unknown error");
        return false;
      }
    });

    response.subscribed.clear();
    for(const auto& log_event : ranged) {

      if(Action::kFollowed == log_event.action) {
        response.subscribed.push_back(log_event.user);
      }

    }

    response.unsubscribed.clear();
    for(const auto& log_event : ranged) {

      if(Action::kUnfollowed == log_event.action) {
        response.unsubscribed.push_back(log_event.user);
      }

    }

  } else {
    SPDLOG_WARN("GetFollowing request, not found any following data for username {}", request.username);
    response.error = -2;
  }

  return response;
}

server::follower::types::GetClientTargetsResponse FollowerAggregator::Impl::GetClientTargets(
  const server::follower::types::GetClientTargetsRequest& request) {
  SPDLOG_INFO("Get client targets request, client: {}", request.client);
  server::follower::types::GetClientTargetsResponse response;

  auto task_info = storage_manager_.RestoreTaskInfo();
  for(auto& item : task_info) {
    try{

      if(std::get<0>(item) == request.client) {
        auto target = server::follower::types::Target {
          .username = std::get<1>(item),
          .followers = std::get<2>(item),
          .following = std::get<3>(item),
          .frequency = static_cast<server::follower::types::PollFrequency>(std::get<4>(item))
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

FollowerAggregator::~FollowerAggregator() = default;

FollowerAggregator::FollowerAggregator()
  : impl_(std::make_unique<Impl>(
      std::make_shared<service::storage::mongo::MongoClient>(
        std::string("mongodb://localhost:27017"),
        std::string("follower-service-test-db"),
        std::string("follower-service-task-collection")),
      std::make_shared<service::storage::mongo::MongoClient>(
        std::string("mongodb://localhost:27017"),
        std::string("follower-service-test-db"),
        std::string("follower-service-followers-collection")),
      std::make_shared<service::storage::mongo::MongoClient>(
        std::string("mongodb://localhost:27017"),
        std::string("follower-service-test-db"),
        std::string("follower-service-followers-log-collection")),
      std::make_shared<service::storage::mongo::MongoClient>(
        std::string("mongodb://localhost:27017"),
        std::string("follower-service-test-db"),
        std::string("follower-service-following-collection")),
      std::make_shared<service::storage::mongo::MongoClient>(
        std::string("mongodb://localhost:27017"),
        std::string("follower-service-test-db"),
        std::string("follower-service-following-log-collection")),
      std::make_shared<service::storage::mongo::MongoObjectCreator>())) {
}

server::follower::types::SubscribeResponse FollowerAggregator::Subscribe(
  const server::follower::types::SubscribeRequest& request) {
  server::follower::types::SubscribeResponse response;

  if(nullptr != impl_) {
    response = impl_->Subscribe(request);
  } else {
    response.error = -2;
  }

  return response;
}

server::follower::types::UnsubscribeResponse FollowerAggregator::Unsubscribe(
  const server::follower::types::UnsubscribeRequest& request) {
  server::follower::types::UnsubscribeResponse response;

  if(nullptr != impl_) {
    response = impl_->Unsubscribe(request);
  } else {
    response.error = -2;
  }

  return response;
}

server::follower::types::GetFollowersResponse FollowerAggregator::GetFollowers(
  const server::follower::types::GetFollowersRequest& request) {
  server::follower::types::GetFollowersResponse response;

  if(nullptr != impl_) {
    response = impl_->GetFollowers(request);
  } else {
    response.error = -2;
  }

  return response;
}

server::follower::types::GetFollowingResponse FollowerAggregator::GetFollowing(
  const server::follower::types::GetFollowingRequest& request) {
  server::follower::types::GetFollowingResponse response;

  if(nullptr != impl_) {
    response = impl_->GetFollowing(request);
  } else {
    response.error = -2;
  }

  return response;
}

server::follower::types::GetClientTargetsResponse FollowerAggregator::GetClientTargets(
  const server::follower::types::GetClientTargetsRequest& request) {
  server::follower::types::GetClientTargetsResponse response;

  if(nullptr != impl_) {
    response = impl_->GetClientTargets(request);
  } else {
    response.error = -2;
  }

  return response;
}

} // service::follower::aggregator
