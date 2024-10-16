#include "FollowerServer.hpp"

#include "Logger.hpp"

#include <grpcxx/server.h>
#include "follower/v1/follower.grpcxx.pb.h"

using namespace follower::v1::FollowerService;

struct FollowerServiceImpl {
  FollowerServiceImpl(std::unique_ptr<server::follower::ServerDelegate> delegate)
    : delegate_(std::move(delegate)) {
  }

  template <typename T>
  typename T::result_type call(grpcxx::context&, const typename T::request_type&) {
    return {grpcxx::status::code_t::unimplemented, std::nullopt};
  }

  std::unique_ptr<server::follower::ServerDelegate> delegate_;
};

template <>
rpcSubscribe::result_type FollowerServiceImpl::call<rpcSubscribe>(
  grpcxx::context &, const follower::v1::SubscribeRequest& request) {
  server::follower::types::SubscribeRequest local_request;
  local_request.client = request.client();
  local_request.target.username = request.target().username();
  local_request.target.followers = request.target().followers();
  local_request.target.following = request.target().following();
  local_request.target.frequency = static_cast<server::follower::types::PollFrequency>(request.target().frequency());

  SPDLOG_INFO("Subscribe request: [client: {}, username: {}, followers: {}, following: {}, frequency: {}]",
    local_request.client,
    local_request.target.username,
    local_request.target.followers,
    local_request.target.following,
    static_cast<int>(local_request.target.frequency));

  rpcSubscribe::result_type result;
  server::follower::types::SubscribeResponse local_response;
  follower::v1::SubscribeResponse response;

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
rpcUnsubscribe::result_type FollowerServiceImpl::call<rpcUnsubscribe>(
  grpcxx::context &, const follower::v1::UnsubscribeRequest& request) {
  server::follower::types::UnsubscribeRequest local_request;
  local_request.client = request.client();
  local_request.username = request.username();

  SPDLOG_INFO("Unsubscribe request: [client: {}, username: {}]",
    local_request.client,
    local_request.username);

  rpcUnsubscribe::result_type result;
  server::follower::types::UnsubscribeResponse local_response;
  follower::v1::UnsubscribeResponse response;

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
rpcGetFollowers::result_type FollowerServiceImpl::call<rpcGetFollowers>(
  grpcxx::context &, const follower::v1::GetFollowersRequest& request) {
  server::follower::types::GetFollowersRequest local_request;
  local_request.client = request.client();
  local_request.username = request.username();
  local_request.from.date = request.from().date();
  local_request.from.time = request.from().time();
  local_request.to.date = request.to().date();
  local_request.to.time = request.to().time();

  SPDLOG_INFO("GetFollowers request: [client: {}, username: {}, from.date: {}, from.time: {}, to.date: {}, to.time: {}]",
    local_request.client,
    local_request.username,
    local_request.from.date,
    local_request.from.time,
    local_request.to.date,
    local_request.to.time);

  rpcGetFollowers::result_type result;
  server::follower::types::GetFollowersResponse local_response;
  follower::v1::GetFollowersResponse response;

  try {
    local_response = delegate_->GetFollowers(local_request);

    for(const auto& follower : local_response.all) {
      auto* proto_follower = response.add_all();
      proto_follower->set_username(follower.username);
      proto_follower->set_full_name(follower.full_name);
      proto_follower->set_pic_url(follower.pic_url);
      auto* timestamp = proto_follower->mutable_timestamp();
      timestamp->set_date(follower.timestamp.date);
      timestamp->set_time(follower.timestamp.time);
    }

    for(const auto& follower : local_response.subscribed) {
      auto* proto_follower = response.add_subscribed();
      proto_follower->set_username(follower.username);
      proto_follower->set_full_name(follower.full_name);
      proto_follower->set_pic_url(follower.pic_url);
      auto* timestamp = proto_follower->mutable_timestamp();
      timestamp->set_date(follower.timestamp.date);
      timestamp->set_time(follower.timestamp.time);
    }

    for(const auto& follower : local_response.unsubscribed) {
      auto* proto_follower = response.add_unsubscribed();
      proto_follower->set_username(follower.username);
      proto_follower->set_full_name(follower.full_name);
      proto_follower->set_pic_url(follower.pic_url);
      auto* timestamp = proto_follower->mutable_timestamp();
      timestamp->set_date(follower.timestamp.date);
      timestamp->set_time(follower.timestamp.time);
    }

    response.set_error_code(local_response.error);

    SPDLOG_INFO("GetFollowers response: [all: {}, subscribed: {}, unsubscribed: {}, error: {}]",
      local_response.all.size(),
      local_response.subscribed.size(),
      local_response.unsubscribed.size(),
      local_response.error);

    result = {
      grpcxx::status::code_t::ok, response
    };
  } catch(...) {
    SPDLOG_ERROR("GetFollowers request failed");
    result = {
      grpcxx::status::code_t::resource_exhausted, response
    };
  }

  return result;
}

template <>
rpcGetFollowing::result_type FollowerServiceImpl::call<rpcGetFollowing>(
  grpcxx::context &, const follower::v1::GetFollowingRequest& request) {
  server::follower::types::GetFollowingRequest local_request;
  local_request.client = request.client();
  local_request.username = request.username();
  local_request.from.date = request.from().date();
  local_request.from.time = request.from().time();
  local_request.to.date = request.to().date();
  local_request.to.time = request.to().time();

  SPDLOG_INFO("GetFollowing request: [client: {}, username: {}, from.date: {}, from.time: {}, to.date: {}, to.time: {}]",
    local_request.client,
    local_request.username,
    local_request.from.date,
    local_request.from.time,
    local_request.to.date,
    local_request.to.time);

  rpcGetFollowing::result_type result;
  server::follower::types::GetFollowingResponse local_response;
  follower::v1::GetFollowingResponse response;

  try {
    local_response = delegate_->GetFollowing(local_request);

    for(const auto& follower : local_response.all) {
      auto* proto_follower = response.add_all();
      proto_follower->set_username(follower.username);
      proto_follower->set_full_name(follower.full_name);
      proto_follower->set_pic_url(follower.pic_url);
      auto* timestamp = proto_follower->mutable_timestamp();
      timestamp->set_date(follower.timestamp.date);
      timestamp->set_time(follower.timestamp.time);
    }

    for(const auto& follower : local_response.subscribed) {
      auto* proto_follower = response.add_subscribed();
      proto_follower->set_username(follower.username);
      proto_follower->set_full_name(follower.full_name);
      proto_follower->set_pic_url(follower.pic_url);
      auto* timestamp = proto_follower->mutable_timestamp();
      timestamp->set_date(follower.timestamp.date);
      timestamp->set_time(follower.timestamp.time);
    }

    for(const auto& follower : local_response.unsubscribed) {
      auto* proto_follower = response.add_unsubscribed();
      proto_follower->set_username(follower.username);
      proto_follower->set_full_name(follower.full_name);
      proto_follower->set_pic_url(follower.pic_url);
      auto* timestamp = proto_follower->mutable_timestamp();
      timestamp->set_date(follower.timestamp.date);
      timestamp->set_time(follower.timestamp.time);
    }

    response.set_error_code(local_response.error);

    SPDLOG_INFO("GetFollowing response: [all: {}, subscribed: {}, unsubscribed: {}, error: {}]",
      local_response.all.size(),
      local_response.subscribed.size(),
      local_response.unsubscribed.size(),
      local_response.error);

    result = {
      grpcxx::status::code_t::ok, response
    };
  } catch(...) {
    SPDLOG_ERROR("GetFollowing request failed");
    result = {
      grpcxx::status::code_t::resource_exhausted, response
    };
  }

  return result;
}

template <>
rpcGetClientTargets::result_type FollowerServiceImpl::call<rpcGetClientTargets>(
  grpcxx::context &, const follower::v1::GetClientTargetsRequest& request) {
  server::follower::types::GetClientTargetsRequest local_request;
  local_request.client = request.client();

  SPDLOG_INFO("GetClientTargets request: [client: {}]", local_request.client);

  rpcGetClientTargets::result_type result;
  server::follower::types::GetClientTargetsResponse local_response;
  follower::v1::GetClientTargetsResponse response;

  try {
    local_response = delegate_->GetClientTargets(local_request);

    for(const auto& local_target : local_response.targets) {
      auto* proto_target = response.add_targets();
      proto_target->set_username(local_target.username);
      proto_target->set_followers(local_target.followers);
      proto_target->set_following(local_target.following);
      proto_target->set_frequency(static_cast<follower::v1::PollFrequency>(local_target.frequency));
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

namespace server::follower {

class FollowerServer::Impl {
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
    FollowerServiceImpl service_impl_;
    Service service_;
    grpcxx::server server_;
};

FollowerServer::Impl::Impl(
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

FollowerServer::Error FollowerServer::Impl::Run() {
  SPDLOG_INFO("Server running");
  server_.run(host_, port_);
  return FollowerServer::Error::eOK;
}

FollowerServer::~FollowerServer() = default;

FollowerServer::FollowerServer(
  const std::string& host,
  const std::size_t& port,
  std::unique_ptr<ServerDelegate> delegate)
  : impl_(std::make_unique<Impl>(host, port, std::move(delegate))) {
}

FollowerServer::Error FollowerServer::Run() {
  return impl_->Run();
}

} // server::follower
