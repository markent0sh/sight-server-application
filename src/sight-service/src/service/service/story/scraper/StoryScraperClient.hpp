#pragma once

#include "StoryScraperTypes.hpp"

namespace service::story::scraper {

std::vector<StoryInfo> get_stories(const std::string& target, Error& error);
std::vector<StoryData> download_stories(const std::string& target, const std::vector<std::string>& stories, Error& error);

} // service::story::scraper

