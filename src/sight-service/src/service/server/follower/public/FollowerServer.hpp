#pragma once

#include <memory>
#include <string>

#include "FollowerTypes.hpp"

namespace server::follower {

class ServerDelegate {
  public:
    virtual ~ServerDelegate() {}
    virtual types::SubscribeResponse Subscribe(
      const types::SubscribeRequest&) = 0;
    virtual types::UnsubscribeResponse Unsubscribe(
      const types::UnsubscribeRequest&) = 0;
    virtual types::GetFollowersResponse GetFollowers(
      const types::GetFollowersRequest&) = 0;
    virtual types::GetFollowingResponse GetFollowing(
      const types::GetFollowingRequest&) = 0;
    virtual types::GetClientTargetsResponse GetClientTargets(
      const types::GetClientTargetsRequest&) = 0;
};

class FollowerServer {
  public:
    enum class Error {
      eOK = 0,
      eNOK = 1
    };

    explicit FollowerServer(
      const std::string&,
      const std::size_t&,
      std::unique_ptr<ServerDelegate>);
    ~FollowerServer();

    Error Run();
  private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // server::follower::types
