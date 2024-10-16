#pragma once

#include "StoryServer.hpp"

namespace service::story::resolver {

class ResolverDelegate {
  public:
    virtual ~ResolverDelegate() {}
    virtual server::story::types::SubscribeResponse Subscribe(
      const server::story::types::SubscribeRequest&) = 0;
    virtual server::story::types::UnsubscribeResponse Unsubscribe(
      const server::story::types::UnsubscribeRequest&) = 0;
    virtual server::story::types::GetAvailableStoriesResponse GetAvailableStories(
      const server::story::types::GetAvailableStoriesRequest&) = 0;
    virtual server::story::types::GetClientTargetsResponse GetClientTargets(
      const server::story::types::GetClientTargetsRequest&) = 0;
};

class StoryResolver : public server::story::ServerDelegate {
  public:
    explicit StoryResolver(
      std::unique_ptr<ResolverDelegate>);

    server::story::types::SubscribeResponse Subscribe(
      const server::story::types::SubscribeRequest&) override;
    server::story::types::UnsubscribeResponse Unsubscribe(
      const server::story::types::UnsubscribeRequest&) override;
    server::story::types::GetAvailableStoriesResponse GetAvailableStories(
      const server::story::types::GetAvailableStoriesRequest&) override;
    server::story::types::GetClientTargetsResponse GetClientTargets(
      const server::story::types::GetClientTargetsRequest&) override;
  private:
    std::unique_ptr<ResolverDelegate> delegate_;
};

} // service::story::resolver
