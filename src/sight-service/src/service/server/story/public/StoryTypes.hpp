#pragma once

#include <string>
#include <vector>

namespace server::story::types {

enum class PollFrequency {
  eRARE = 0,
  eMEDIUM = 1,
  eFAST = 2,
  eVERY_FAST = 3,
  eQUANTUM = 4
};

struct Target {
  std::string username;
  PollFrequency frequency;
};

struct Timestamp {
  std::string date;
  std::string time;
};

struct Story {
  Timestamp timestamp;
  std::string uri;
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

struct GetAvailableStoriesRequest {
  int64_t client;
  std::string username;
};

struct GetAvailableStoriesResponse {
  std::vector<Story> stories;
  int32_t error;
};

struct GetClientTargetsRequest {
  int64_t client;
};

struct GetClientTargetsResponse {
  std::vector<Target> targets;
  int32_t error;
};

} // server::story::types
