#pragma once

#include "FollowerScraperTypes.hpp"

namespace service::follower::scraper {

std::vector<UserInfo> get_target_following(const std::string& target, Error& error);
std::vector<UserInfo> get_target_followers(const std::string& target, Error& error);

} // service::follower::scraper
