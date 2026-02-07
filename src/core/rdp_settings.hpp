#pragma once

#include "config/connection_profile.hpp"

#include <freerdp/freerdp.h>

namespace gvrdp {

// Maps a ConnectionProfile to FreeRDP settings on the rdpContext.
bool apply_profile_to_settings(rdpSettings* settings, const ConnectionProfile& profile);

}  // namespace gvrdp
