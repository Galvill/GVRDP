#pragma once

#include "config/connection_profile.hpp"
#include "core/rdp_error.hpp"

#include <SDL2/SDL.h>

#include <functional>
#include <string>

namespace gvrdp {

class SdlRenderer;

enum class UiState {
    ConnectionDialog,
    Connecting,
    Connected,
    OverlayVisible,
    ErrorDialog,
};

// Manages Dear ImGui lifecycle, event routing, and UI state machine.
class UiManager {
public:
    using ConnectCallback = std::function<void(const ConnectionProfile&)>;
    using DisconnectCallback = std::function<void()>;

    UiManager();
    ~UiManager();

    bool init(SdlRenderer& renderer);
    void shutdown();

    // Process SDL event. Returns true if ImGui consumed it.
    bool process_event(const SDL_Event& event);

    // Render the current UI state
    void render(SdlRenderer& renderer);

    // State transitions
    void set_state(UiState state);
    UiState state() const { return state_; }
    void show_error(const std::string& message);
    void set_connecting();
    void set_connected();
    void set_disconnected();

    // Hotkey check (Ctrl+Shift+S for overlay toggle)
    bool check_overlay_toggle(const SDL_Event& event);

    // Callbacks
    void set_connect_callback(ConnectCallback cb) { on_connect_ = std::move(cb); }
    void set_disconnect_callback(DisconnectCallback cb) { on_disconnect_ = std::move(cb); }

    // Profile access
    ConnectionProfile& current_profile() { return current_profile_; }
    const ConnectionProfile& current_profile() const { return current_profile_; }

private:
    UiState state_ = UiState::ConnectionDialog;
    ConnectionProfile current_profile_;
    std::string error_message_;
    ConnectCallback on_connect_;
    DisconnectCallback on_disconnect_;
    bool imgui_initialized_ = false;
};

}  // namespace gvrdp
