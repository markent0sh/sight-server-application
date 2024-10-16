#pragma once

#include <memory>
#include <string>

#include "StoryTypes.hpp"

namespace server::story {

class ServerDelegate {
  public:
    virtual ~ServerDelegate() {}
    virtual types::SubscribeResponse Subscribe(
      const types::SubscribeRequest&) = 0;
    virtual types::UnsubscribeResponse Unsubscribe(
      const types::UnsubscribeRequest&) = 0;
    virtual types::GetAvailableStoriesResponse GetAvailableStories(
      const types::GetAvailableStoriesRequest&) = 0;
    virtual types::GetClientTargetsResponse GetClientTargets(
      const types::GetClientTargetsRequest&) = 0;
};

class StoryServer {
  public:
    enum class Error {
      eOK = 0,
      eNOK = 1
    };

    explicit StoryServer(
      const std::string&,
      const std::size_t&,
      std::unique_ptr<ServerDelegate>);
    ~StoryServer();

    Error Run();
  private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // server::story::types
