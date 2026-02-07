#include "config/app_config.hpp"

#include "util/logger.hpp"

#include <fstream>

namespace gvrdp {

AppConfig AppConfig::load(const std::filesystem::path& config_dir) {
    auto path = config_dir / "config.json";
    AppConfig config;
    try {
        std::ifstream file(path);
        if (file.is_open()) {
            nlohmann::json j;
            file >> j;
            config = j.get<AppConfig>();
        }
    } catch (const std::exception& e) {
        LOG_WARN("Failed to load app config: {}", e.what());
    }
    return config;
}

void AppConfig::save(const std::filesystem::path& config_dir) const {
    try {
        std::error_code ec;
        std::filesystem::create_directories(config_dir, ec);
        auto path = config_dir / "config.json";
        std::ofstream file(path);
        if (file.is_open()) {
            nlohmann::json j = *this;
            file << j.dump(4);
        }
    } catch (const std::exception& e) {
        LOG_WARN("Failed to save app config: {}", e.what());
    }
}

}  // namespace gvrdp
