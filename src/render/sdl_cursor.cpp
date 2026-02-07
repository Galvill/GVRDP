#include "render/sdl_cursor.hpp"

#include "util/logger.hpp"

namespace gvrdp {

SdlCursor::SdlCursor() = default;

SdlCursor::~SdlCursor() {
    reset();
}

void SdlCursor::update(const uint8_t* data, uint32_t width, uint32_t height, uint32_t hotspot_x,
                        uint32_t hotspot_y) {
    reset();

    if (!data || width == 0 || height == 0) return;

    surface_ = SDL_CreateRGBSurfaceFrom(const_cast<uint8_t*>(data), static_cast<int>(width),
                                         static_cast<int>(height), 32,
                                         static_cast<int>(width * 4), 0x00FF0000, 0x0000FF00,
                                         0x000000FF, 0xFF000000);
    if (!surface_) {
        LOG_WARN("Failed to create cursor surface: {}", SDL_GetError());
        return;
    }

    cursor_ = SDL_CreateColorCursor(surface_, static_cast<int>(hotspot_x),
                                     static_cast<int>(hotspot_y));
    if (cursor_) {
        SDL_SetCursor(cursor_);
    }
}

void SdlCursor::set_visible(bool visible) {
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

void SdlCursor::reset() {
    if (cursor_) {
        SDL_SetCursor(SDL_GetDefaultCursor());
        SDL_FreeCursor(cursor_);
        cursor_ = nullptr;
    }
    if (surface_) {
        SDL_FreeSurface(surface_);
        surface_ = nullptr;
    }
}

}  // namespace gvrdp
