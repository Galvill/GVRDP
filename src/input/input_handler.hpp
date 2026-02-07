#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

namespace gvrdp {

class RdpSession;

// Translates SDL input events into RDP input calls.
class InputHandler {
public:
    explicit InputHandler(RdpSession& session);

    // Process an SDL event. Returns true if the event was consumed.
    bool handle_event(const SDL_Event& event);

    // Get the last known window size from resize events
    uint32_t pending_width() const { return pending_width_; }
    uint32_t pending_height() const { return pending_height_; }
    bool has_pending_resize() const { return has_pending_resize_; }
    void clear_pending_resize() { has_pending_resize_ = false; }

private:
    void handle_key_event(const SDL_KeyboardEvent& key);
    void handle_mouse_motion(const SDL_MouseMotionEvent& motion);
    void handle_mouse_button(const SDL_MouseButtonEvent& button);
    void handle_mouse_wheel(const SDL_MouseWheelEvent& wheel);
    void handle_window_event(const SDL_WindowEvent& window);

    RdpSession& session_;
    uint32_t pending_width_ = 0;
    uint32_t pending_height_ = 0;
    bool has_pending_resize_ = false;
};

}  // namespace gvrdp
