#include "FollowerScraperClient.hpp"

#include "Logger.hpp"

#include <sstream>
#include <iomanip>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

namespace {
  std::string FLASK_SERVER_URL("http://localhost:5000");
  long SUCCESS_CODE = 200;
}

namespace service::follower::scraper {

std::vector<UserInfo> get_target_followers(const std::string& target, Error& error) {
  std::vector<UserInfo> result;
  error = Error::eOK;

  nlohmann::json get_target_followers_request = {
    {"target", target}
  };

  try {
    auto response = cpr::Post(
      cpr::Url{FLASK_SERVER_URL + "/get_target_followers"},
      cpr::Body{get_target_followers_request.dump()},
      cpr::Header{{"Content-Type", "application/json"}});

    auto response_json = nlohmann::json::parse(response.text);
    for(auto& follower_json : response_json["followers"]) {
      UserInfo follower;
      follower.username = follower_json["username"];
      follower.full_name = follower_json["name"];
      follower.pic_url = follower_json["pic_url"];
      result.push_back(follower);
    }

    if(SUCCESS_CODE != response.status_code) {
      error = Error::eNOK;
    }

  } catch (const std::exception& e) {
    SPDLOG_ERROR("Get target followers failed, error: {}", e.what());
    error = Error::eNOK;
  }

  return result;
}

std::vector<UserInfo> get_target_following(const std::string& target, Error& error) {
  std::vector<UserInfo> result;
  error = Error::eOK;

  nlohmann::json get_target_following_request = {
    {"target", target}
  };

  try {
    auto response = cpr::Post(
      cpr::Url{FLASK_SERVER_URL + "/get_target_following"},
      cpr::Body{get_target_following_request.dump()},
      cpr::Header{{"Content-Type", "application/json"}});

    auto response_json = nlohmann::json::parse(response.text);
    for(auto& following_json : response_json["following"]) {
      UserInfo following;
      following.username = following_json["username"];
      following.full_name = following_json["name"];
      following.pic_url = following_json["pic_url"];
      result.push_back(following);
    }

    if(SUCCESS_CODE != response.status_code) {
      error = Error::eNOK;
    }

  } catch (const std::exception& e) {
    SPDLOG_ERROR("Get target following failed, error: {}", e.what());
    error = Error::eNOK;
  }

  return result;
}

} // service::follower::scraper
