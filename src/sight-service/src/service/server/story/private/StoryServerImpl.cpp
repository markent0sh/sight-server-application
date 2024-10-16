#include "StoryServer.hpp"

#include "Logger.hpp"

#include <grpcxx/server.h>
#include "story/v1/story.grpcxx.pb.h"

using namespace story::v1::StoryService;

struct StoryServiceImpl {
  StoryServiceImpl(std::unique_ptr<server::story::ServerDelegate> delegate)
    : delegate_(std::move(delegate)) {
  }

  template <typename T>
  typename T::result_type call(grpcxx::context&, const typename T::request_type&) {
    return {grpcxx::status::code_t::unimplemented, std::nullopt};
  }

  std::unique_ptr<server::story::ServerDelegate> delegate_;
};

template <>
rpcSubscribe::result_type StoryServiceImpl::call<rpcSubscribe>(
  grpcxx::context &, const story::v1::SubscribeRequest& request) {
  server::story::types::SubscribeRequest local_request;
  local_request.client = request.client();
  local_request.target.username = request.target().username();
  local_request.target.frequency = static_cast<server::story::types::PollFrequency>(request.target().frequency());

  SPDLOG_INFO("Subscribe request: [client: {}, username: {}, frequency: {}]",
    local_request.client,
    local_request.target.username,
    static_cast<int>(local_request.target.frequency));

  rpcSubscribe::result_type result;
  server::story::types::SubscribeResponse local_response;
  story::v1::SubscribeResponse response;

  try {
    local_response = delegate_->Subscribe(local_request);
    response.set_error_code(local_response.error);

    SPDLOG_INFO("Subscribe response: [error: {}]", local_response.error);

    result = {
      grpcxx::status::code_t::ok, response
    };
  } catch(...) {
    SPDLOG_ERROR("Subscribe request failed");
    result = {
      grpcxx::status::code_t::resource_exhausted, response
    };
  }

  return result;
}

template <>
rpcUnsubscribe::result_type StoryServiceImpl::call<rpcUnsubscribe>(
  grpcxx::context &, const story::v1::UnsubscribeRequest& request) {
  server::story::types::UnsubscribeRequest local_request;
  local_request.client = request.client();
  local_request.username = request.username();

  SPDLOG_INFO("Unsubscribe request: [client: {}, username: {}]",
    local_request.client,
    local_request.username);

  rpcUnsubscribe::result_type result;
  server::story::types::UnsubscribeResponse local_response;
  story::v1::UnsubscribeResponse response;

  try {
    local_response = delegate_->Unsubscribe(local_request);
    response.set_error_code(local_response.error);

    SPDLOG_INFO("Unsubscribe response: [error: {}]",
      local_response.error);

    result = {
      grpcxx::status::code_t::ok, response
    };
  } catch(...) {
    SPDLOG_ERROR("Unsubscribe request failed");
    result = {
      grpcxx::status::code_t::resource_exhausted, response
    };
  }

  return result;
}

template <>
rpcGetAvailableStories::result_type StoryServiceImpl::call<rpcGetAvailableStories>(
  grpcxx::context &, const story::v1::GetAvailableStoriesRequest& request) {
  server::story::types::GetAvailableStoriesRequest local_request;
  local_request.client = request.client();
  local_request.username = request.username();

  SPDLOG_INFO("GetAvailableStories request: [client: {}, username: {}]",
    local_request.client,
    local_request.username);

  rpcGetAvailableStories::result_type result;
  server::story::types::GetAvailableStoriesResponse local_response;
  story::v1::GetAvailableStoriesResponse response;

  try {
    local_response = delegate_->GetAvailableStories(local_request);

    for(const auto& local_story : local_response.stories) {
      auto* proto_story = response.add_stories();
      auto* timestamp = proto_story->mutable_timestamp();
      timestamp->set_date(local_story.timestamp.date);
      timestamp->set_time(local_story.timestamp.time);
      proto_story->set_uri(local_story.uri);
    }

    response.set_error_code(local_response.error);

    SPDLOG_INFO("GetAvailableStories response: [stories: {}, error: {}]",
      local_response.stories.size(),
      local_response.error);

    result = {
      grpcxx::status::code_t::ok, response
    };
  } catch(...) {
    SPDLOG_ERROR("GetAvailableStories request failed");
    result = {
      grpcxx::status::code_t::resource_exhausted, response
    };
  }

  return result;
}

template <>
rpcGetClientTargets::result_type StoryServiceImpl::call<rpcGetClientTargets>(
  grpcxx::context &, const story::v1::GetClientTargetsRequest& request) {
  server::story::types::GetClientTargetsRequest local_request;
  local_request.client = request.client();

  SPDLOG_INFO("GetClientTargets request: [client: {}]", local_request.client);

  rpcGetClientTargets::result_type result;
  server::story::types::GetClientTargetsResponse local_response;
  story::v1::GetClientTargetsResponse response;

  try {
    local_response = delegate_->GetClientTargets(local_request);

    for(const auto& local_target : local_response.targets) {
      auto* proto_target = response.add_targets();
      proto_target->set_username(local_target.username);
      proto_target->set_frequency(static_cast<story::v1::PollFrequency>(local_target.frequency));
    }

    response.set_error_code(local_response.error);

    SPDLOG_INFO("GetClientTargets response: [targets: {}, error: {}]",
      local_response.targets.size(),
      local_response.error);

    result = {
      grpcxx::status::code_t::ok, response
    };
  } catch(...) {
    SPDLOG_ERROR("GetClientTargets request failed");
    result = {
      grpcxx::status::code_t::resource_exhausted, response
    };
  }

  return result;
}

namespace server::story {

class StoryServer::Impl {
  public:
    explicit Impl(
      const std::string&,
      const std::size_t&,
      std::unique_ptr<ServerDelegate>);
    ~Impl() = default;

    Error Run();
  private:
    std::string host_;
    std::size_t port_;
    StoryServiceImpl service_impl_;
    Service service_;
    grpcxx::server server_;
};

StoryServer::Impl::Impl(
  const std::string& host,
  const std::size_t& port,
  std::unique_ptr<ServerDelegate> delegate)
  : host_(host),
    port_(port),
    service_impl_(std::move(delegate)),
    service_(service_impl_),
    server_() {
  SPDLOG_INFO("host: {}, port: {}", host_, port_);
  server_.add(service_);
}

StoryServer::Error StoryServer::Impl::Run() {
  SPDLOG_INFO("Server running");
  server_.run(host_, port_);
  return StoryServer::Error::eOK;
}

StoryServer::~StoryServer() = default;

StoryServer::StoryServer(
  const std::string& host,
  const std::size_t& port,
  std::unique_ptr<ServerDelegate> delegate)
  : impl_(std::make_unique<Impl>(host, port, std::move(delegate))) {
}

StoryServer::Error StoryServer::Run() {
  return impl_->Run();
}

} // server::story
