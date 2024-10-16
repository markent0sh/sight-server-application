#include <iostream>
#include <map>

#include "FollowerServer.hpp"

class FollowerServerDelegate : public server::follower::ServerDelegate {
  public:
    server::follower::types::SubscribeResponse Subscribe(
      const server::follower::types::SubscribeRequest&) override;
    server::follower::types::UnsubscribeResponse Unsubscribe(
      const server::follower::types::UnsubscribeRequest&) override;
    server::follower::types::GetFollowersResponse GetFollowers(
      const server::follower::types::GetFollowersRequest&) override;
    server::follower::types::GetFollowingResponse GetFollowing(
      const server::follower::types::GetFollowingRequest&) override;
    server::follower::types::GetClientTargetsResponse GetClientTargets(
      const server::follower::types::GetClientTargetsRequest&) {}
};

server::follower::types::SubscribeResponse FollowerServerDelegate::Subscribe(
  const server::follower::types::SubscribeRequest& request) {
  std::cout << "FollowerServerDelegate::Subscribe(client: " << request.client
    << ", username: " << request.target.username
    << ", followers: " << request.target.followers
    << ", following: " << request.target.following
    << ")" << std::endl;

  server::follower::types::SubscribeResponse response;
  response.error = 0;
  return response;
}

server::follower::types::UnsubscribeResponse FollowerServerDelegate::Unsubscribe(
  const server::follower::types::UnsubscribeRequest& request) {
  std::cout << "FollowerServerDelegate::Unsubscribe(client: " << request.client
    << ", username: " << request.username << ")" << std::endl;

  server::follower::types::UnsubscribeResponse response;
  response.error = 0;
  return response;
}

server::follower::types::GetFollowersResponse FollowerServerDelegate::GetFollowers(
  const server::follower::types::GetFollowersRequest& request) {
  std::cout << "FollowerServerDelegate::GetFollowers(client: " << request.client
    << ", username: " << request.username
    << ", from: " << request.from.date << " - " << request.from.time
    << ", to: " << request.to.date << " - " << request.to.time
    << ")" << std::endl;

  server::follower::types::GetFollowersResponse response;
  response.error = 0;

  std::size_t date_counter = 1000;
  std::size_t time_counter = 5000;
  std::size_t un_counter = 10000;
  for(auto i = 0; i < 10; i++) {
    server::follower::types::Timestamp ts;
    ts.date = std::string(std::to_string(date_counter++));
    ts.time = std::string(std::to_string(time_counter++));

    server::follower::types::Follower st;
    st.timestamp = ts;
    st.username = std::string(std::to_string(un_counter++));

    response.all.push_back(st);
  }

  response.subscribed.insert(
    response.subscribed.begin(),
      response.all.begin(), response.all.begin() + 3);

  response.unsubscribed.insert(
    response.unsubscribed.begin(),
      response.all.begin(), response.all.begin() + 5);

  return response;
}

server::follower::types::GetFollowingResponse FollowerServerDelegate::GetFollowing(
  const server::follower::types::GetFollowingRequest& request) {
  std::cout << "FollowerServerDelegate::GetFollowing(client: " << request.client
    << ", username: " << request.username
    << ", from: " << request.from.date << " - " << request.from.time
    << ", to: " << request.to.date << " - " << request.to.time
    << ")" << std::endl;

  server::follower::types::GetFollowingResponse response;
  response.error = 0;

  std::size_t date_counter = 1000;
  std::size_t time_counter = 5000;
  std::size_t un_counter = 10000;
  for(auto i = 0; i < 10; i++) {
    server::follower::types::Timestamp ts;
    ts.date = std::string(std::to_string(date_counter++));
    ts.time = std::string(std::to_string(time_counter++));

    server::follower::types::Follower st;
    st.timestamp = ts;
    st.username = std::string(std::to_string(un_counter++));

    response.all.push_back(st);
  }

  response.subscribed.insert(
    response.subscribed.begin(),
      response.all.begin(), response.all.begin() + 3);

  response.unsubscribed.insert(
    response.unsubscribed.begin(),
      response.all.begin(), response.all.begin() + 5);

  return response;
}

int main() {
  server::follower::FollowerServer server(
    std::string("127.0.0.1"), 33333,
    std::make_unique<FollowerServerDelegate>());

  auto result = server.Run();
  std::cout << "Run result: " << static_cast<int>(result) << std::endl;
  return 0;
}
