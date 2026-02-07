#include "input/keyboard_map.hpp"

#include <freerdp/input.h>

#include <unordered_map>

namespace gvrdp {

// SDL scancode to RDP scancode mapping table.
// RDP uses the same scancodes as PS/2 keyboard scan set 1.
static const std::unordered_map<SDL_Scancode, RdpScancode> kScanMap = {
    // Letters
    {SDL_SCANCODE_A, {0x1E, false}},
    {SDL_SCANCODE_B, {0x30, false}},
    {SDL_SCANCODE_C, {0x2E, false}},
    {SDL_SCANCODE_D, {0x20, false}},
    {SDL_SCANCODE_E, {0x12, false}},
    {SDL_SCANCODE_F, {0x21, false}},
    {SDL_SCANCODE_G, {0x22, false}},
    {SDL_SCANCODE_H, {0x23, false}},
    {SDL_SCANCODE_I, {0x17, false}},
    {SDL_SCANCODE_J, {0x24, false}},
    {SDL_SCANCODE_K, {0x25, false}},
    {SDL_SCANCODE_L, {0x26, false}},
    {SDL_SCANCODE_M, {0x32, false}},
    {SDL_SCANCODE_N, {0x31, false}},
    {SDL_SCANCODE_O, {0x18, false}},
    {SDL_SCANCODE_P, {0x19, false}},
    {SDL_SCANCODE_Q, {0x10, false}},
    {SDL_SCANCODE_R, {0x13, false}},
    {SDL_SCANCODE_S, {0x1F, false}},
    {SDL_SCANCODE_T, {0x14, false}},
    {SDL_SCANCODE_U, {0x16, false}},
    {SDL_SCANCODE_V, {0x2F, false}},
    {SDL_SCANCODE_W, {0x11, false}},
    {SDL_SCANCODE_X, {0x2D, false}},
    {SDL_SCANCODE_Y, {0x15, false}},
    {SDL_SCANCODE_Z, {0x2C, false}},

    // Numbers
    {SDL_SCANCODE_1, {0x02, false}},
    {SDL_SCANCODE_2, {0x03, false}},
    {SDL_SCANCODE_3, {0x04, false}},
    {SDL_SCANCODE_4, {0x05, false}},
    {SDL_SCANCODE_5, {0x06, false}},
    {SDL_SCANCODE_6, {0x07, false}},
    {SDL_SCANCODE_7, {0x08, false}},
    {SDL_SCANCODE_8, {0x09, false}},
    {SDL_SCANCODE_9, {0x0A, false}},
    {SDL_SCANCODE_0, {0x0B, false}},

    // Function keys
    {SDL_SCANCODE_F1, {0x3B, false}},
    {SDL_SCANCODE_F2, {0x3C, false}},
    {SDL_SCANCODE_F3, {0x3D, false}},
    {SDL_SCANCODE_F4, {0x3E, false}},
    {SDL_SCANCODE_F5, {0x3F, false}},
    {SDL_SCANCODE_F6, {0x40, false}},
    {SDL_SCANCODE_F7, {0x41, false}},
    {SDL_SCANCODE_F8, {0x42, false}},
    {SDL_SCANCODE_F9, {0x43, false}},
    {SDL_SCANCODE_F10, {0x44, false}},
    {SDL_SCANCODE_F11, {0x57, false}},
    {SDL_SCANCODE_F12, {0x58, false}},

    // Special keys
    {SDL_SCANCODE_ESCAPE, {0x01, false}},
    {SDL_SCANCODE_TAB, {0x0F, false}},
    {SDL_SCANCODE_CAPSLOCK, {0x3A, false}},
    {SDL_SCANCODE_LSHIFT, {0x2A, false}},
    {SDL_SCANCODE_RSHIFT, {0x36, false}},
    {SDL_SCANCODE_LCTRL, {0x1D, false}},
    {SDL_SCANCODE_RCTRL, {0x1D, true}},
    {SDL_SCANCODE_LALT, {0x38, false}},
    {SDL_SCANCODE_RALT, {0x38, true}},
    {SDL_SCANCODE_LGUI, {0x5B, true}},
    {SDL_SCANCODE_RGUI, {0x5C, true}},
    {SDL_SCANCODE_RETURN, {0x1C, false}},
    {SDL_SCANCODE_SPACE, {0x39, false}},
    {SDL_SCANCODE_BACKSPACE, {0x0E, false}},

    // Punctuation
    {SDL_SCANCODE_MINUS, {0x0C, false}},
    {SDL_SCANCODE_EQUALS, {0x0D, false}},
    {SDL_SCANCODE_LEFTBRACKET, {0x1A, false}},
    {SDL_SCANCODE_RIGHTBRACKET, {0x1B, false}},
    {SDL_SCANCODE_BACKSLASH, {0x2B, false}},
    {SDL_SCANCODE_SEMICOLON, {0x27, false}},
    {SDL_SCANCODE_APOSTROPHE, {0x28, false}},
    {SDL_SCANCODE_GRAVE, {0x29, false}},
    {SDL_SCANCODE_COMMA, {0x33, false}},
    {SDL_SCANCODE_PERIOD, {0x34, false}},
    {SDL_SCANCODE_SLASH, {0x35, false}},

    // Navigation
    {SDL_SCANCODE_INSERT, {0x52, true}},
    {SDL_SCANCODE_DELETE, {0x53, true}},
    {SDL_SCANCODE_HOME, {0x47, true}},
    {SDL_SCANCODE_END, {0x4F, true}},
    {SDL_SCANCODE_PAGEUP, {0x49, true}},
    {SDL_SCANCODE_PAGEDOWN, {0x51, true}},
    {SDL_SCANCODE_UP, {0x48, true}},
    {SDL_SCANCODE_DOWN, {0x50, true}},
    {SDL_SCANCODE_LEFT, {0x4B, true}},
    {SDL_SCANCODE_RIGHT, {0x4D, true}},

    // Numpad
    {SDL_SCANCODE_NUMLOCKCLEAR, {0x45, false}},
    {SDL_SCANCODE_KP_DIVIDE, {0x35, true}},
    {SDL_SCANCODE_KP_MULTIPLY, {0x37, false}},
    {SDL_SCANCODE_KP_MINUS, {0x4A, false}},
    {SDL_SCANCODE_KP_PLUS, {0x4E, false}},
    {SDL_SCANCODE_KP_ENTER, {0x1C, true}},
    {SDL_SCANCODE_KP_0, {0x52, false}},
    {SDL_SCANCODE_KP_1, {0x4F, false}},
    {SDL_SCANCODE_KP_2, {0x50, false}},
    {SDL_SCANCODE_KP_3, {0x51, false}},
    {SDL_SCANCODE_KP_4, {0x4B, false}},
    {SDL_SCANCODE_KP_5, {0x4C, false}},
    {SDL_SCANCODE_KP_6, {0x4D, false}},
    {SDL_SCANCODE_KP_7, {0x47, false}},
    {SDL_SCANCODE_KP_8, {0x48, false}},
    {SDL_SCANCODE_KP_9, {0x49, false}},
    {SDL_SCANCODE_KP_PERIOD, {0x53, false}},

    // Lock keys
    {SDL_SCANCODE_SCROLLLOCK, {0x46, false}},
    {SDL_SCANCODE_PRINTSCREEN, {0x37, true}},
    {SDL_SCANCODE_PAUSE, {0x45, true}},

    // Application key
    {SDL_SCANCODE_APPLICATION, {0x5D, true}},
};

RdpScancode sdl_scancode_to_rdp(SDL_Scancode scancode) {
    auto it = kScanMap.find(scancode);
    if (it != kScanMap.end()) {
        return it->second;
    }
    return {0, false};
}

}  // namespace gvrdp
