#pragma once

#include <nlohmann/json.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace gvrdp {

struct ConnectionProfile {
    std::string name = "Untitled";
    std::string hostname;
    uint16_t port = 3389;
    std::string username;
    std::string domain;
    // Password is NOT persisted to disk for security
    std::string password;

    // Display
    uint32_t width = 1920;
    uint32_t height = 1080;
    uint32_t color_depth = 32;
    bool fullscreen = false;
    bool dynamic_resolution = true;

    // Channels
    bool enable_clipboard = true;
    bool enable_audio = true;
    bool enable_drive_redirect = false;
    std::string drive_redirect_path;

    // Performance
    bool enable_wallpaper = false;
    bool enable_font_smoothing = true;
    bool enable_desktop_composition = false;
    bool enable_themes = true;

    // Security
    bool ignore_certificate = false;
    std::string gateway_hostname;
    uint16_t gateway_port = 443;
    std::string gateway_username;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        ConnectionProfile,
        name, hostname, port, username, domain,
        width, height, color_depth, fullscreen, dynamic_resolution,
        enable_clipboard, enable_audio, enable_drive_redirect, drive_redirect_path,
        enable_wallpaper, enable_font_smoothing, enable_desktop_composition, enable_themes,
        ignore_certificate, gateway_hostname, gateway_port, gateway_username
    )
};

}  // namespace gvrdp
