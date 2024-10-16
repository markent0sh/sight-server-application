#include "FollowerResolver.hpp"

namespace service::follower::resolver {

FollowerResolver::FollowerResolver(
  std::unique_ptr<ResolverDelegate> delegate)
  : delegate_(std::move(delegate)) {
}

server::follower::types::SubscribeResponse FollowerResolver::Subscribe(
  const server::follower::types::SubscribeRequest& request) {
  server::follower::types::SubscribeResponse response;

  if(nullptr != delegate_) {
    response = delegate_->Subscribe(request);
  } else {
    response.error = -1;
  }

  return response;
}

server::follower::types::UnsubscribeResponse FollowerResolver::Unsubscribe(
  const server::follower::types::UnsubscribeRequest& request) {
  server::follower::types::UnsubscribeResponse response;

  if(nullptr != delegate_) {
    response = delegate_->Unsubscribe(request);
  } else {
    response.error = -1;
  }

  return response;
}

server::follower::types::GetFollowersResponse FollowerResolver::GetFollowers(
  const server::follower::types::GetFollowersRequest& request) {
  server::follower::types::GetFollowersResponse response;

  if(nullptr != delegate_) {
    response = delegate_->GetFollowers(request);
  } else {
    response.error = -1;
  }

  return response;
}

server::follower::types::GetFollowingResponse FollowerResolver::GetFollowing(
  const server::follower::types::GetFollowingRequest& request) {
  server::follower::types::GetFollowingResponse response;

  if(nullptr != delegate_) {
    response = delegate_->GetFollowing(request);
  } else {
    response.error = -1;
  }

  return response;
}

server::follower::types::GetClientTargetsResponse FollowerResolver::GetClientTargets(
  const server::follower::types::GetClientTargetsRequest& request) {
  server::follower::types::GetClientTargetsResponse response;

  if(nullptr != delegate_) {
    response = delegate_->GetClientTargets(request);
  } else {
    response.error = -1;
  }

  return response;
}

} // service::follower::resolver
