#pragma once

#include <string>
#include <vector>

namespace service::story::scraper {

enum class Error {
  eOK,
  eNOK
};

struct StickerInfo {
  std::string direct_url;
  std::string display_url;
};

struct StoryInfo {
  std::string id;
  std::string video_url;
  std::string photo_url;
  std::string date;
  std::string time;
  std::vector<StickerInfo> stickers;
};

struct StoryData {
  std::string id;
  std::string uri;
};

} // service::story::scraper

