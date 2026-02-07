#include "core/rdp_settings.hpp"

#include "util/logger.hpp"

#include <freerdp/settings.h>

namespace gvrdp {

bool apply_profile_to_settings(rdpSettings* settings, const ConnectionProfile& profile) {
    if (!settings) return false;

    // Server
    if (!freerdp_settings_set_string(settings, FreeRDP_ServerHostname, profile.hostname.c_str()))
        return false;
    if (!freerdp_settings_set_uint32(settings, FreeRDP_ServerPort, profile.port))
        return false;

    // Credentials
    if (!profile.username.empty()) {
        if (!freerdp_settings_set_string(settings, FreeRDP_Username, profile.username.c_str()))
            return false;
    }
    if (!profile.password.empty()) {
        if (!freerdp_settings_set_string(settings, FreeRDP_Password, profile.password.c_str()))
            return false;
    }
    if (!profile.domain.empty()) {
        if (!freerdp_settings_set_string(settings, FreeRDP_Domain, profile.domain.c_str()))
            return false;
    }

    // Display
    if (!freerdp_settings_set_uint32(settings, FreeRDP_DesktopWidth, profile.width))
        return false;
    if (!freerdp_settings_set_uint32(settings, FreeRDP_DesktopHeight, profile.height))
        return false;
    if (!freerdp_settings_set_uint32(settings, FreeRDP_ColorDepth, profile.color_depth))
        return false;
    if (!freerdp_settings_set_bool(settings, FreeRDP_Fullscreen, profile.fullscreen))
        return false;

    // Dynamic resolution
    if (!freerdp_settings_set_bool(settings, FreeRDP_SupportDisplayControl,
                                   profile.dynamic_resolution))
        return false;
    if (!freerdp_settings_set_bool(settings, FreeRDP_DynamicResolutionUpdate,
                                   profile.dynamic_resolution))
        return false;

    // Software GDI (required for buffer access)
    if (!freerdp_settings_set_bool(settings, FreeRDP_SoftwareGdi, TRUE))
        return false;

    // Performance flags
    if (!freerdp_settings_set_bool(settings, FreeRDP_DisableWallpaper, !profile.enable_wallpaper))
        return false;
    if (!freerdp_settings_set_bool(settings, FreeRDP_AllowFontSmoothing,
                                   profile.enable_font_smoothing))
        return false;
    if (!freerdp_settings_set_bool(settings, FreeRDP_AllowDesktopComposition,
                                   profile.enable_desktop_composition))
        return false;
    if (!freerdp_settings_set_bool(settings, FreeRDP_DisableThemes, !profile.enable_themes))
        return false;

    // Auto-logon
    if (!profile.username.empty() && !profile.password.empty()) {
        if (!freerdp_settings_set_bool(settings, FreeRDP_AutoLogonEnabled, TRUE))
            return false;
    }

    // Certificate handling
    if (!freerdp_settings_set_bool(settings, FreeRDP_IgnoreCertificate, profile.ignore_certificate))
        return false;

    LOG_INFO("Applied profile settings: {}:{} as {}", profile.hostname, profile.port,
             profile.username.empty() ? "(no user)" : profile.username);
    return true;
}

}  // namespace gvrdp
