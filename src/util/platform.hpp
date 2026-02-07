#pragma once

// Platform detection macros
#if defined(_WIN32) || defined(_WIN64)
#define GVRDP_WINDOWS 1
#define GVRDP_LINUX 0
#define GVRDP_MACOS 0
#elif defined(__APPLE__)
#define GVRDP_WINDOWS 0
#define GVRDP_LINUX 0
#define GVRDP_MACOS 1
#elif defined(__linux__)
#define GVRDP_WINDOWS 0
#define GVRDP_LINUX 1
#define GVRDP_MACOS 0
#else
#error "Unsupported platform"
#endif

#include <filesystem>
#include <string>

namespace gvrdp {

inline std::filesystem::path get_config_dir() {
#if GVRDP_WINDOWS
    const char* appdata = std::getenv("APPDATA");
    if (appdata) return std::filesystem::path(appdata) / "GVRDP";
    return std::filesystem::path(".") / ".gvrdp";
#elif GVRDP_MACOS
    const char* home = std::getenv("HOME");
    if (home) return std::filesystem::path(home) / "Library" / "Application Support" / "GVRDP";
    return std::filesystem::path(".") / ".gvrdp";
#else
    // XDG Base Directory
    const char* xdg = std::getenv("XDG_CONFIG_HOME");
    if (xdg) return std::filesystem::path(xdg) / "gvrdp";
    const char* home = std::getenv("HOME");
    if (home) return std::filesystem::path(home) / ".config" / "gvrdp";
    return std::filesystem::path(".") / ".gvrdp";
#endif
}

}  // namespace gvrdp
