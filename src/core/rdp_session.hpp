#pragma once

#include "config/connection_profile.hpp"
#include "core/rdp_context.hpp"
#include "core/rdp_error.hpp"

#include <freerdp/freerdp.h>

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>

struct SDL_UserEvent;

namespace gvrdp {

class DispChannel;

// Custom SDL user event types
enum GvrdpEvent : int {
    GVRDP_EVENT_FRAME_READY = 0,
    GVRDP_EVENT_DISCONNECT,
    GVRDP_EVENT_RESIZE,
    GVRDP_EVENT_ERROR,
};

class RdpSession {
public:
    RdpSession();
    ~RdpSession();

    RdpSession(const RdpSession&) = delete;
    RdpSession& operator=(const RdpSession&) = delete;

    // Lifecycle
    bool connect(const ConnectionProfile& profile, uint32_t sdl_window_id);
    void disconnect();
    bool is_connected() const;
    RdpError last_error() const;

    // Called from main thread
    void request_resolution_change(uint32_t width, uint32_t height);

    // Input forwarding (thread-safe)
    void send_keyboard_event(uint16_t flags, uint8_t code);
    void send_mouse_event(uint16_t flags, uint16_t x, uint16_t y);
    void send_extended_mouse_event(uint16_t flags, uint16_t x, uint16_t y);

    // GDI buffer access (for frame copy on main thread)
    const uint8_t* gdi_buffer() const;
    uint32_t gdi_width() const;
    uint32_t gdi_height() const;
    uint32_t gdi_stride() const;

    // Callbacks invoked by C trampolines
    bool on_pre_connect();
    bool on_post_connect();
    void on_post_disconnect();
    bool on_begin_paint();
    bool on_end_paint();
    bool on_desktop_resize();
    uint32_t on_verify_certificate(const char* host, uint16_t port, const char* common_name,
                                   const char* subject, const char* issuer, const char* fingerprint,
                                   uint32_t flags);

    // Channel management
    void on_channel_connected(const char* name, void* iface);
    void on_channel_disconnected(const char* name, void* iface);

    // Disp channel access
    DispChannel* disp_channel() const { return disp_channel_.get(); }

    // SDL window event ID for pushing events
    uint32_t sdl_window_id() const { return sdl_window_id_; }

private:
    void rdp_thread_func();
    void push_sdl_event(GvrdpEvent type, int code = 0, void* data1 = nullptr);

    freerdp* instance_ = nullptr;
    GvrdpContext* context_ = nullptr;
    std::thread rdp_thread_;
    std::atomic<bool> connected_{false};
    std::atomic<bool> should_disconnect_{false};
    RdpError last_error_ = RdpError::None;
    ConnectionProfile profile_;
    uint32_t sdl_window_id_ = 0;
    std::mutex send_mutex_;

    // Channel objects
    std::unique_ptr<DispChannel> disp_channel_;

    // Certificate auto-accept flag
    bool ignore_certificate_ = false;
};

}  // namespace gvrdp
