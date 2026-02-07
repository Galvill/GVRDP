#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>

namespace gvrdp {

struct AppConfig {
    std::string log_level = "info";
    std::string last_profile;
    int window_x = -1;  // -1 = centered
    int window_y = -1;
    int window_w = 1280;
    int window_h = 720;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        AppConfig,
        log_level, last_profile, window_x, window_y, window_w, window_h
    )

    static AppConfig load(const std::filesystem::path& config_dir);
    void save(const std::filesystem::path& config_dir) const;
};

}  // namespace gvrdp
