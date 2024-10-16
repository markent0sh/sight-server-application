#pragma once

#include "FollowerServer.hpp"

namespace service::follower::resolver {

class ResolverDelegate {
  public:
    virtual ~ResolverDelegate() {}
    virtual server::follower::types::SubscribeResponse Subscribe(
      const server::follower::types::SubscribeRequest&) = 0;
    virtual server::follower::types::UnsubscribeResponse Unsubscribe(
      const server::follower::types::UnsubscribeRequest&) = 0;
    virtual server::follower::types::GetFollowersResponse GetFollowers(
      const server::follower::types::GetFollowersRequest&) = 0;
    virtual server::follower::types::GetFollowingResponse GetFollowing(
      const server::follower::types::GetFollowingRequest&) = 0;
    virtual server::follower::types::GetClientTargetsResponse GetClientTargets(
      const server::follower::types::GetClientTargetsRequest&) = 0;
};

class FollowerResolver : public server::follower::ServerDelegate {
  public:
    explicit FollowerResolver(
      std::unique_ptr<ResolverDelegate>);

    server::follower::types::SubscribeResponse Subscribe(
      const server::follower::types::SubscribeRequest&) override;
    server::follower::types::UnsubscribeResponse Unsubscribe(
      const server::follower::types::UnsubscribeRequest&) override;
    server::follower::types::GetFollowersResponse GetFollowers(
      const server::follower::types::GetFollowersRequest&) override;
    server::follower::types::GetFollowingResponse GetFollowing(
      const server::follower::types::GetFollowingRequest&) override;
    server::follower::types::GetClientTargetsResponse GetClientTargets(
      const server::follower::types::GetClientTargetsRequest&) override;
  private:
    std::unique_ptr<ResolverDelegate> delegate_;
};

} // service::follower::resolver
