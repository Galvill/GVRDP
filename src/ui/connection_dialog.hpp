#pragma once

#include "config/connection_profile.hpp"

#include <functional>

namespace gvrdp {

// Draw the connection dialog using ImGui.
// Modifies the profile in-place and calls on_connect when the user clicks Connect.
void draw_connection_dialog(ConnectionProfile& profile,
                            const std::function<void(const ConnectionProfile&)>& on_connect);

}  // namespace gvrdp
