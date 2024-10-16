#pragma once

#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace service::common::time {

struct Timestamp {
  std::size_t day;
  std::size_t month;
  std::size_t year;
  std::size_t hours;
  std::size_t minutes;

  Timestamp(const std::string& date_string, const std::string& time_string) {
    std::istringstream dateStream(date_string);
    std::istringstream timeStream(time_string);
    char date_delimiter, time_delimiter;

    if(!(dateStream >> day >> date_delimiter >> month >> date_delimiter >> year) ||
      !(timeStream >> hours >> time_delimiter >> minutes) ||
      date_delimiter != '.' || time_delimiter != ':' ||
      day < 1 || day > 31 || month < 1 || month > 12 ||
      hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60) {
      throw std::invalid_argument("Invalid timestamp format or value: " + date_string + " " + time_string);
    }

    bool is_leap_year = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    std::vector<std::size_t> days_in_month = {31, std::size_t(is_leap_year ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(day > days_in_month[month - 1]) {
      throw std::invalid_argument("Invalid day for the given month: " + date_string);
    }

  }

  bool operator>(const Timestamp& other) const {
    return std::tie(year, month, day, hours, minutes) > std::tie(other.year, other.month, other.day, other.hours, other.minutes);
  }

  bool operator<(const Timestamp& other) const {
    return std::tie(year, month, day, hours, minutes) < std::tie(other.year, other.month, other.day, other.hours, other.minutes);
  }

  bool operator==(const Timestamp& other) const {
    return std::tie(year, month, day, hours, minutes) == std::tie(other.year, other.month, other.day, other.hours, other.minutes);
  }

  bool operator>=(const Timestamp& other) const {
    return !(*this < other);
  }

  bool operator<=(const Timestamp& other) const {
    return !(*this > other);
  }
};

std::pair<std::string, std::string> get_current_utc_time() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_t = std::chrono::system_clock::to_time_t(now);
  std::tm* utc_tm = std::gmtime(&now_t);

  std::stringstream date_stream;
  std::stringstream time_stream;

  date_stream << std::put_time(utc_tm, "%d.%m.%Y");
  std::string date = date_stream.str();

  time_stream << std::put_time(utc_tm, "%H:%M");
  std::string time = time_stream.str();

  return {date, time};
}

} // service::common::time
