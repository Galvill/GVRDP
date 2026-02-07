#pragma once

#include <SDL2/SDL.h>

#include <cstdint>
#include <string>

namespace gvrdp {

// Manages the SDL2 window, renderer, and texture for displaying the remote desktop.
class SdlRenderer {
public:
    SdlRenderer();
    ~SdlRenderer();

    SdlRenderer(const SdlRenderer&) = delete;
    SdlRenderer& operator=(const SdlRenderer&) = delete;

    // Create window and renderer
    bool init(const std::string& title, int x, int y, int w, int h);
    void shutdown();

    // Recreate texture at new dimensions (called on desktop resize)
    bool resize_texture(uint32_t width, uint32_t height);

    // Copy GDI buffer data into the texture
    void update_frame(const uint8_t* buffer, uint32_t width, uint32_t height, uint32_t stride);

    // Render the desktop texture to the window
    void render_desktop();

    // Present the final frame (call after ImGui render)
    void present();

    // Clear the renderer
    void clear();

    SDL_Window* window() const { return window_; }
    SDL_Renderer* renderer() const { return renderer_; }
    uint32_t window_id() const;

    int window_width() const;
    int window_height() const;

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;
    uint32_t tex_width_ = 0;
    uint32_t tex_height_ = 0;
};

}  // namespace gvrdp
