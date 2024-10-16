#include <iostream>
#include <map>

#include "StoryServer.hpp"

class StoryServerDelegate : public server::story::ServerDelegate {
  public:
    server::story::types::SubscribeResponse Subscribe(
      const server::story::types::SubscribeRequest&) override;
    server::story::types::UnsubscribeResponse Unsubscribe(
      const server::story::types::UnsubscribeRequest&) override;
    server::story::types::GetAvailableStoriesResponse GetAvailableStories(
      const server::story::types::GetAvailableStoriesRequest&) override;
    server::story::types::GetClientTargetsResponse GetClientTargets(
      const server::story::types::GetClientTargetsRequest&) {}
};

server::story::types::SubscribeResponse StoryServerDelegate::Subscribe(
  const server::story::types::SubscribeRequest& request) {
  std::cout << "StoryServerDelegate::Subscribe(client: " << request.client
    << ", username: " << request.target.username << ")" << std::endl;

  server::story::types::SubscribeResponse response;
  response.error = 0;
  return response;
}

server::story::types::UnsubscribeResponse StoryServerDelegate::Unsubscribe(
  const server::story::types::UnsubscribeRequest& request) {
  std::cout << "StoryServerDelegate::Unsubscribe(client: " << request.client
    << ", username: " << request.username << ")" << std::endl;

  server::story::types::UnsubscribeResponse response;
  response.error = 0;
  return response;
}

server::story::types::GetAvailableStoriesResponse StoryServerDelegate::GetAvailableStories(
  const server::story::types::GetAvailableStoriesRequest& request) {
  std::cout << "StoryServerDelegate::GetAvailableStories(client: " << request.client
    << ", username: " << request.username << ")" << std::endl;

  server::story::types::GetAvailableStoriesResponse response;
  response.error = 0;

  std::size_t date_counter = 1000;
  std::size_t time_counter = 5000;
  std::size_t uri_counter = 10000;
  for(auto i = 0; i < 10; i++) {
    server::story::types::Timestamp ts;
    ts.date = std::string(std::to_string(date_counter++));
    ts.time = std::string(std::to_string(time_counter++));

    server::story::types::Story st;
    st.timestamp = ts;
    st.uri = std::string(std::to_string(uri_counter++));

    response.stories.push_back(st);
  }

  return response;
}

int main() {
  server::story::StoryServer server(
    std::string("127.0.0.1"), 33333,
    std::make_unique<StoryServerDelegate>());

  auto result = server.Run();
  std::cout << "Run result: " << static_cast<int>(result) << std::endl;
  return 0;
}
