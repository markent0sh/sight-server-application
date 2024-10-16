#pragma once

#include "StoryResolver.hpp"

namespace service::story::aggregator {

class StoryAggregator : public service::story::resolver::ResolverDelegate {
  public:
    StoryAggregator();
    ~StoryAggregator();
    StoryAggregator(StoryAggregator&&) = default;
    StoryAggregator& operator=(StoryAggregator&&) = default;
    StoryAggregator(const StoryAggregator&) = delete;
    StoryAggregator& operator=(const StoryAggregator&) = delete;

    server::story::types::SubscribeResponse Subscribe(
      const server::story::types::SubscribeRequest&) override;
    server::story::types::UnsubscribeResponse Unsubscribe(
      const server::story::types::UnsubscribeRequest&) override;
    server::story::types::GetAvailableStoriesResponse GetAvailableStories(
      const server::story::types::GetAvailableStoriesRequest&) override;
    server::story::types::GetClientTargetsResponse GetClientTargets(
      const server::story::types::GetClientTargetsRequest&) override;
  private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // service::story::aggregator
