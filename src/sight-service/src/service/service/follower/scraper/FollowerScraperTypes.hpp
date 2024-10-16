#pragma once

#include <string>
#include <vector>

namespace service::follower::scraper {

enum class Error {
  eOK,
  eNOK
};

struct UserInfo {
  std::string username;
  std::string full_name;
  std::string pic_url;
};

} // service::follower::scraper

