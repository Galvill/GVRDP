#pragma once

#include "config/connection_profile.hpp"

#include <functional>

namespace gvrdp {

// Draw the in-session settings overlay (toggled by Ctrl+Shift+S).
void draw_settings_dialog(ConnectionProfile& profile,
                          const std::function<void()>& on_disconnect);

}  // namespace gvrdp
