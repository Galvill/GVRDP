#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace gvrdp {

class Logger {
public:
    static void init(const std::string& log_level = "info");
    static std::shared_ptr<spdlog::logger>& get();

private:
    static std::shared_ptr<spdlog::logger> logger_;
};

}  // namespace gvrdp

// Convenience macros
#define LOG_TRACE(...) SPDLOG_LOGGER_TRACE(::gvrdp::Logger::get(), __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(::gvrdp::Logger::get(), __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_INFO(::gvrdp::Logger::get(), __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_WARN(::gvrdp::Logger::get(), __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(::gvrdp::Logger::get(), __VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::gvrdp::Logger::get(), __VA_ARGS__)
