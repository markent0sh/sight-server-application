#include "StoryResolver.hpp"

namespace service::story::resolver {

StoryResolver::StoryResolver(
  std::unique_ptr<ResolverDelegate> delegate)
  : delegate_(std::move(delegate)) {
}

server::story::types::SubscribeResponse StoryResolver::Subscribe(
  const server::story::types::SubscribeRequest& request) {
  server::story::types::SubscribeResponse response;

  if(nullptr != delegate_) {
    response = delegate_->Subscribe(request);
  } else {
    response.error = -1;
  }

  return response;
}

server::story::types::UnsubscribeResponse StoryResolver::Unsubscribe(
  const server::story::types::UnsubscribeRequest& request) {
  server::story::types::UnsubscribeResponse response;

  if(nullptr != delegate_) {
    response = delegate_->Unsubscribe(request);
  } else {
    response.error = -1;
  }

  return response;
}

server::story::types::GetAvailableStoriesResponse StoryResolver::GetAvailableStories(
  const server::story::types::GetAvailableStoriesRequest& request) {
  server::story::types::GetAvailableStoriesResponse response;

  if(nullptr != delegate_) {
    response = delegate_->GetAvailableStories(request);
  } else {
    response.error = -1;
  }

  return response;
}

server::story::types::GetClientTargetsResponse StoryResolver::GetClientTargets(
  const server::story::types::GetClientTargetsRequest& request) {
  server::story::types::GetClientTargetsResponse response;

  if(nullptr != delegate_) {
    response = delegate_->GetClientTargets(request);
  } else {
    response.error = -1;
  }

  return response;
}

} // service::story::resolver
