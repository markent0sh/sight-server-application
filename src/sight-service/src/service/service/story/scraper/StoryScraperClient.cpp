#include "StoryScraperClient.hpp"

#include "Logger.hpp"

#include <sstream>
#include <iomanip>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

namespace {
  std::string FLASK_SERVER_URL("http://localhost:5000");
  long SUCCESS_CODE = 200;
}

namespace service::story::scraper {

std::vector<StoryInfo> get_stories(const std::string& target, Error& error) {
  std::vector<StoryInfo> result;
  error = Error::eOK;

  nlohmann::json get_stories_request = {
    {"target", target}
  };

  try {
    auto response = cpr::Post(
      cpr::Url{FLASK_SERVER_URL + "/get_stories"},
      cpr::Body{get_stories_request.dump()},
      cpr::Header{{"Content-Type", "application/json"}});

    auto response_json = nlohmann::json::parse(response.text);
    for(auto& story_json : response_json["stories"]) {
      StoryInfo story;
      story.id = story_json["id"];
      story.video_url = story_json["video_url"];
      story.photo_url = story_json["photo_url"];

      std::istringstream ss(story_json["timestamp"].get<std::string>());
      std::tm tm = {};
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      std::ostringstream date_stream, time_stream;
      date_stream << std::put_time(&tm, "%d.%m.%Y");
      time_stream << std::put_time(&tm, "%H:%M");
      story.date = date_stream.str();
      story.time = time_stream.str();

      for (auto& sticker_json : story_json["stickers"]) {
        StickerInfo sticker;
        sticker.direct_url = sticker_json["direct_url"];
        sticker.display_url = sticker_json["display_url"];
        story.stickers.push_back(sticker);
      }

      result.push_back(story);
    }

    if(SUCCESS_CODE != response.status_code) {
      error = Error::eNOK;
    }

  } catch (const std::exception& e) {
    SPDLOG_ERROR("Get stories failed, error: {}", e.what());
    error = Error::eNOK;
  }

  return result;
}

std::vector<StoryData> download_stories(const std::string& target, const std::vector<std::string>& stories, Error& error) {
  std::vector<StoryData> result;
  error = Error::eOK;

  nlohmann::json request = {
    {"target", target},
    {"stories", stories}
  };

  try {
    auto response = cpr::Post(
      cpr::Url{FLASK_SERVER_URL + "/download_stories"},
      cpr::Body{request.dump()},
      cpr::Header{{"Content-Type", "application/json"}});

    auto result_json = nlohmann::json::parse(response.text);

    for(const auto& item : result_json["stories"]) {
      StoryData data;
      data.id = item["id"];
      data.uri = item["uri"];
      result.push_back(data);
    }

    if(SUCCESS_CODE != response.status_code) {
      error = Error::eNOK;
    }

  } catch (const std::exception& e) {
    SPDLOG_ERROR("Download stories failed, error: {}", e.what());
    error = Error::eNOK;
  }

  return result;
}

} // service::story::scraper
