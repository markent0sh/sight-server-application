#pragma once

#include <string>
#include <vector>

namespace server::follower::types {

struct Timestamp {
  std::string date;
  std::string time;
};

struct Follower {
  std::string username;
  std::string full_name;
  std::string pic_url;
  Timestamp timestamp;

  bool operator==(const Follower& p) const {
    return username == p.username;
  }
};

enum class PollFrequency {
  eRARE = 0,
  eMEDIUM = 1,
  eFAST = 2,
  eVERY_FAST = 3,
  eQUANTUM = 4
};

struct Target {
  std::string username;
  bool followers;
  bool following;
  PollFrequency frequency;
};

struct SubscribeRequest {
  int64_t client;
  Target target;
};

struct SubscribeResponse {
  int32_t error;
};

struct UnsubscribeRequest {
  int64_t client;
  std::string username;
};

struct UnsubscribeResponse {
  int32_t error;
};

struct GetFollowersRequest {
  int64_t client;
  std::string username;
  Timestamp from;
  Timestamp to;
};

struct GetFollowersResponse {
  std::vector<Follower> all; // current
  std::vector<Follower> subscribed; // from - to
  std::vector<Follower> unsubscribed; // from - to
  int32_t error;
};

struct GetFollowingRequest {
  int64_t client;
  std::string username;
  Timestamp from;
  Timestamp to;
};

struct GetFollowingResponse {
  std::vector<Follower> all; // current
  std::vector<Follower> subscribed; // from - to
  std::vector<Follower> unsubscribed; // from - to
  int32_t error;
};

struct GetClientTargetsRequest {
  int64_t client;
};

struct GetClientTargetsResponse {
  std::vector<Target> targets;
  int32_t error;
};

} // server::follower::types
