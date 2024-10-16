#pragma once

#include "FollowerResolver.hpp"

namespace service::follower::aggregator {

class FollowerAggregator : public service::follower::resolver::ResolverDelegate {
  public:
    FollowerAggregator();
    ~FollowerAggregator();
    FollowerAggregator(FollowerAggregator&&) = default;
    FollowerAggregator& operator=(FollowerAggregator&&) = default;
    FollowerAggregator(const FollowerAggregator&) = delete;
    FollowerAggregator& operator=(const FollowerAggregator&) = delete;

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
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // service::follower::aggregator
