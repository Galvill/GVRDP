#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

namespace gvrdp {

// Manages custom cursor rendering for the remote desktop pointer.
class SdlCursor {
public:
    SdlCursor();
    ~SdlCursor();

    SdlCursor(const SdlCursor&) = delete;
    SdlCursor& operator=(const SdlCursor&) = delete;

    // Update cursor image from RDP pointer data
    void update(const uint8_t* data, uint32_t width, uint32_t height, uint32_t hotspot_x,
                uint32_t hotspot_y);

    // Show/hide the custom cursor
    void set_visible(bool visible);

    // Reset to default system cursor
    void reset();

private:
    SDL_Cursor* cursor_ = nullptr;
    SDL_Surface* surface_ = nullptr;
};

}  // namespace gvrdp
