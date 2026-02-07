#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

namespace gvrdp {

// Converts SDL scancodes to RDP scancodes.
struct RdpScancode {
    uint8_t code;
    bool extended;
};

// Returns the RDP scancode for a given SDL scancode.
// Returns {0, false} if unmapped.
RdpScancode sdl_scancode_to_rdp(SDL_Scancode scancode);

}  // namespace gvrdp
