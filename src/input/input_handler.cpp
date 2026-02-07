#include "input/input_handler.hpp"

#include "core/rdp_session.hpp"
#include "input/keyboard_map.hpp"
#include "util/logger.hpp"

#include <freerdp/input.h>

namespace gvrdp {

InputHandler::InputHandler(RdpSession& session) : session_(session) {}

bool InputHandler::handle_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            handle_key_event(event.key);
            return true;
        case SDL_MOUSEMOTION:
            handle_mouse_motion(event.motion);
            return true;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            handle_mouse_button(event.button);
            return true;
        case SDL_MOUSEWHEEL:
            handle_mouse_wheel(event.wheel);
            return true;
        case SDL_WINDOWEVENT:
            handle_window_event(event.window);
            return true;
        default:
            return false;
    }
}

void InputHandler::handle_key_event(const SDL_KeyboardEvent& key) {
    auto rdp_sc = sdl_scancode_to_rdp(key.keysym.scancode);
    if (rdp_sc.code == 0) return;

    uint16_t flags = 0;
    if (key.type == SDL_KEYUP) {
        flags |= KBD_FLAGS_RELEASE;
    }
    if (rdp_sc.extended) {
        flags |= KBD_FLAGS_EXTENDED;
    }

    session_.send_keyboard_event(flags, rdp_sc.code);
}

void InputHandler::handle_mouse_motion(const SDL_MouseMotionEvent& motion) {
    uint16_t flags = PTR_FLAGS_MOVE;
    session_.send_mouse_event(flags, static_cast<uint16_t>(motion.x),
                              static_cast<uint16_t>(motion.y));
}

void InputHandler::handle_mouse_button(const SDL_MouseButtonEvent& button) {
    uint16_t flags = 0;

    switch (button.button) {
        case SDL_BUTTON_LEFT:
            flags = PTR_FLAGS_BUTTON1;
            break;
        case SDL_BUTTON_RIGHT:
            flags = PTR_FLAGS_BUTTON2;
            break;
        case SDL_BUTTON_MIDDLE:
            flags = PTR_FLAGS_BUTTON3;
            break;
        default:
            return;
    }

    if (button.type == SDL_MOUSEBUTTONDOWN) {
        flags |= PTR_FLAGS_DOWN;
    }

    session_.send_mouse_event(flags, static_cast<uint16_t>(button.x),
                              static_cast<uint16_t>(button.y));
}

void InputHandler::handle_mouse_wheel(const SDL_MouseWheelEvent& wheel) {
    if (wheel.y != 0) {
        uint16_t flags = PTR_FLAGS_WHEEL;
        if (wheel.y < 0) {
            flags |= PTR_FLAGS_WHEEL_NEGATIVE;
            flags |= static_cast<uint16_t>((-wheel.y * 120) & 0x01FF);
        } else {
            flags |= static_cast<uint16_t>((wheel.y * 120) & 0x01FF);
        }
        int x, y;
        SDL_GetMouseState(&x, &y);
        session_.send_mouse_event(flags, static_cast<uint16_t>(x), static_cast<uint16_t>(y));
    }

    if (wheel.x != 0) {
        uint16_t flags = PTR_FLAGS_HWHEEL;
        if (wheel.x < 0) {
            flags |= PTR_FLAGS_WHEEL_NEGATIVE;
            flags |= static_cast<uint16_t>((-wheel.x * 120) & 0x01FF);
        } else {
            flags |= static_cast<uint16_t>((wheel.x * 120) & 0x01FF);
        }
        int x, y;
        SDL_GetMouseState(&x, &y);
        session_.send_mouse_event(flags, static_cast<uint16_t>(x), static_cast<uint16_t>(y));
    }
}

void InputHandler::handle_window_event(const SDL_WindowEvent& window) {
    if (window.event == SDL_WINDOWEVENT_RESIZED || window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        pending_width_ = static_cast<uint32_t>(window.data1);
        pending_height_ = static_cast<uint32_t>(window.data2);
        has_pending_resize_ = true;
        LOG_DEBUG("Window resize: {}x{}", pending_width_, pending_height_);
    }
}

}  // namespace gvrdp
