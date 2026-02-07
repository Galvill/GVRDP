#include "ui/profile_manager_dialog.hpp"

#include <imgui.h>

#include <array>

namespace gvrdp {

void draw_profile_manager_dialog(ProfileStore& store, ConnectionProfile& current_profile,
                                 std::vector<ConnectionProfile>& profiles, bool& show) {
    if (!show) return;

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 350), ImGuiCond_Appearing);

    if (!ImGui::Begin("Profile Manager", &show)) {
        ImGui::End();
        return;
    }

    // Save current profile
    static std::array<char, 128> save_name{};
    ImGui::InputText("Profile Name", save_name.data(), save_name.size());
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        std::string name = save_name.data();
        if (!name.empty()) {
            current_profile.name = name;
            store.save(current_profile);
            profiles = store.load_all();
        }
    }

    ImGui::Separator();
    ImGui::Text("Saved Profiles:");
    ImGui::Spacing();

    // List profiles
    int to_delete = -1;
    for (int i = 0; i < static_cast<int>(profiles.size()); i++) {
        ImGui::PushID(i);
        auto& p = profiles[static_cast<size_t>(i)];

        if (ImGui::Selectable(p.name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            // Load on single click
            current_profile = p;
            std::snprintf(save_name.data(), save_name.size(), "%s", p.name.c_str());
        }

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50);
        if (ImGui::SmallButton("Delete")) {
            to_delete = i;
        }

        ImGui::PopID();
    }

    if (to_delete >= 0) {
        store.remove(profiles[static_cast<size_t>(to_delete)].name);
        profiles = store.load_all();
    }

    if (profiles.empty()) {
        ImGui::TextDisabled("No saved profiles");
    }

    ImGui::End();
}

}  // namespace gvrdp
