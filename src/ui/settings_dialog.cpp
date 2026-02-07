#include "ui/settings_dialog.hpp"

#include <imgui.h>

namespace gvrdp {

void draw_settings_dialog(ConnectionProfile& profile,
                          const std::function<void()>& on_disconnect) {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0));

    ImGui::Begin("Session Settings", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Connected to: %s:%d", profile.hostname.c_str(), profile.port);
    ImGui::Text("User: %s", profile.username.empty() ? "(none)" : profile.username.c_str());
    ImGui::Separator();

    ImGui::Text("Press Ctrl+Shift+S to close this overlay");
    ImGui::Spacing();

    // Display settings
    if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Dynamic Resolution", &profile.dynamic_resolution);
        ImGui::Checkbox("Font Smoothing", &profile.enable_font_smoothing);
        ImGui::Checkbox("Desktop Composition", &profile.enable_desktop_composition);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Disconnect button
    if (ImGui::Button("Disconnect", ImVec2(-1, 35))) {
        if (on_disconnect) on_disconnect();
    }

    ImGui::End();
}

}  // namespace gvrdp
