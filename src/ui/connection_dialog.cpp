#include "ui/connection_dialog.hpp"

#include <imgui.h>

#include <array>

namespace gvrdp {

void draw_connection_dialog(ConnectionProfile& profile,
                            const std::function<void(const ConnectionProfile&)>& on_connect) {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(480, 0));

    ImGui::Begin("Connect to Remote Desktop", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    // Connection section
    if (ImGui::CollapsingHeader("Connection", ImGuiTreeNodeFlags_DefaultOpen)) {
        static std::array<char, 256> hostname_buf{};
        static std::array<char, 128> username_buf{};
        static std::array<char, 128> password_buf{};
        static std::array<char, 128> domain_buf{};
        static bool initialized = false;

        if (!initialized) {
            std::snprintf(hostname_buf.data(), hostname_buf.size(), "%s",
                          profile.hostname.c_str());
            std::snprintf(username_buf.data(), username_buf.size(), "%s",
                          profile.username.c_str());
            std::snprintf(domain_buf.data(), domain_buf.size(), "%s", profile.domain.c_str());
            initialized = true;
        }

        ImGui::InputText("Hostname", hostname_buf.data(), hostname_buf.size());
        int port = profile.port;
        ImGui::InputInt("Port", &port);
        if (port > 0 && port < 65536) profile.port = static_cast<uint16_t>(port);

        ImGui::Separator();
        ImGui::InputText("Username", username_buf.data(), username_buf.size());
        ImGui::InputText("Password", password_buf.data(), password_buf.size(),
                         ImGuiInputTextFlags_Password);
        ImGui::InputText("Domain", domain_buf.data(), domain_buf.size());

        // Sync buffers to profile on every frame
        profile.hostname = hostname_buf.data();
        profile.username = username_buf.data();
        profile.password = password_buf.data();
        profile.domain = domain_buf.data();
    }

    // Display section
    if (ImGui::CollapsingHeader("Display")) {
        int width = static_cast<int>(profile.width);
        int height = static_cast<int>(profile.height);
        ImGui::InputInt("Width", &width);
        ImGui::InputInt("Height", &height);
        if (width > 0) profile.width = static_cast<uint32_t>(width);
        if (height > 0) profile.height = static_cast<uint32_t>(height);

        ImGui::Checkbox("Dynamic Resolution", &profile.dynamic_resolution);
        ImGui::Checkbox("Fullscreen", &profile.fullscreen);
    }

    // Channels section
    if (ImGui::CollapsingHeader("Channels")) {
        ImGui::Checkbox("Clipboard", &profile.enable_clipboard);
        ImGui::Checkbox("Audio", &profile.enable_audio);
        ImGui::Checkbox("Drive Redirect", &profile.enable_drive_redirect);
    }

    // Performance section
    if (ImGui::CollapsingHeader("Performance")) {
        ImGui::Checkbox("Wallpaper", &profile.enable_wallpaper);
        ImGui::Checkbox("Font Smoothing", &profile.enable_font_smoothing);
        ImGui::Checkbox("Desktop Composition", &profile.enable_desktop_composition);
        ImGui::Checkbox("Themes", &profile.enable_themes);
    }

    // Security section
    if (ImGui::CollapsingHeader("Security")) {
        ImGui::Checkbox("Ignore Certificate Warnings", &profile.ignore_certificate);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Connect button
    bool can_connect = !profile.hostname.empty();
    if (!can_connect) ImGui::BeginDisabled();
    if (ImGui::Button("Connect", ImVec2(-1, 35))) {
        if (on_connect) on_connect(profile);
    }
    if (!can_connect) ImGui::EndDisabled();

    ImGui::End();
}

}  // namespace gvrdp
