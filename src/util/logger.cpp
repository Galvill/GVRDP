#include "util/logger.hpp"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace gvrdp {

std::shared_ptr<spdlog::logger> Logger::logger_;

void Logger::init(const std::string& log_level) {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");

    logger_ = std::make_shared<spdlog::logger>("gvrdp", console_sink);
    logger_->set_level(spdlog::level::from_str(log_level));
    logger_->flush_on(spdlog::level::warn);
    spdlog::set_default_logger(logger_);
}

std::shared_ptr<spdlog::logger>& Logger::get() {
    if (!logger_) {
        init();
    }
    return logger_;
}

}  // namespace gvrdp
