#include "render/sdl_renderer.hpp"

#include "util/logger.hpp"

namespace gvrdp {

SdlRenderer::SdlRenderer() = default;

SdlRenderer::~SdlRenderer() {
    shutdown();
}

bool SdlRenderer::init(const std::string& title, int x, int y, int w, int h) {
    int pos_x = (x >= 0) ? x : static_cast<int>(SDL_WINDOWPOS_CENTERED);
    int pos_y = (y >= 0) ? y : static_cast<int>(SDL_WINDOWPOS_CENTERED);

    window_ = SDL_CreateWindow(title.c_str(), pos_x, pos_y, w, h,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
                                   SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window_) {
        LOG_ERROR("SDL_CreateWindow failed: {}", SDL_GetError());
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1,
                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        LOG_ERROR("SDL_CreateRenderer failed: {}", SDL_GetError());
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        return false;
    }

    LOG_INFO("SDL renderer initialized: {}x{}", w, h);
    return true;
}

void SdlRenderer::shutdown() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
}

bool SdlRenderer::resize_texture(uint32_t width, uint32_t height) {
    if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }

    // BGRA32 from FreeRDP maps to SDL ARGB8888
    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 static_cast<int>(width), static_cast<int>(height));
    if (!texture_) {
        LOG_ERROR("SDL_CreateTexture failed: {}", SDL_GetError());
        return false;
    }

    tex_width_ = width;
    tex_height_ = height;
    LOG_INFO("Texture resized to {}x{}", width, height);
    return true;
}

void SdlRenderer::update_frame(const uint8_t* buffer, uint32_t width, uint32_t height,
                                uint32_t stride) {
    if (!texture_ || !buffer) return;

    // Recreate texture if dimensions changed
    if (width != tex_width_ || height != tex_height_) {
        if (!resize_texture(width, height)) return;
    }

    SDL_UpdateTexture(texture_, nullptr, buffer, static_cast<int>(stride));
}

void SdlRenderer::render_desktop() {
    if (!texture_) return;
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
}

void SdlRenderer::present() {
    SDL_RenderPresent(renderer_);
}

void SdlRenderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
}

uint32_t SdlRenderer::window_id() const {
    return window_ ? SDL_GetWindowID(window_) : 0;
}

int SdlRenderer::window_width() const {
    int w = 0;
    if (window_) SDL_GetWindowSize(window_, &w, nullptr);
    return w;
}

int SdlRenderer::window_height() const {
    int h = 0;
    if (window_) SDL_GetWindowSize(window_, nullptr, &h);
    return h;
}

}  // namespace gvrdp
