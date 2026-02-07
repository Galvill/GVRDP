#pragma once

#include "config/connection_profile.hpp"
#include "config/profile_store.hpp"

#include <functional>
#include <vector>

namespace gvrdp {

// Draw the profile manager dialog for saving/loading/deleting profiles.
void draw_profile_manager_dialog(ProfileStore& store, ConnectionProfile& current_profile,
                                 std::vector<ConnectionProfile>& profiles, bool& show);

}  // namespace gvrdp
