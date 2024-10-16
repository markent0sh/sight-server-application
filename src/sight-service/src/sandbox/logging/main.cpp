#include "Logger.hpp"

int main() {
  SET_LOG_LEVEL();
  spdlog::set_pattern("[%T %z] [%l] [tid: %t] [%s:%!:%#] - %v");

  SPDLOG_INFO("INFO LOG");
  SPDLOG_DEBUG("DEBUG LOG");
  SPDLOG_TRACE("TRACE LOG");

  return 0;
}