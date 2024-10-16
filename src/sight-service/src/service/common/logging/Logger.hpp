#pragma once

#ifdef LOG_LEVEL_INFO
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#define SET_LOG_LEVEL() spdlog::set_level(spdlog::level::info);
#elif LOG_LEVEL_DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#define SET_LOG_LEVEL() spdlog::set_level(spdlog::level::debug);
#elif LOG_LEVEL_TRACE
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SET_LOG_LEVEL() spdlog::set_level(spdlog::level::trace);
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#define SET_LOG_LEVEL() spdlog::set_level(spdlog::level::info);
#endif

#include "spdlog/spdlog.h"
