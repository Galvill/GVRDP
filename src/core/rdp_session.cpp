#include "core/rdp_session.hpp"

#include "channels/disp_channel.hpp"
#include "core/rdp_callbacks.hpp"
#include "core/rdp_channels.hpp"
#include "core/rdp_settings.hpp"
#include "util/logger.hpp"

#include <freerdp/client/channels.h>
#include <freerdp/client/cliprdr.h>
#include <freerdp/client/cmdline.h>
#include <freerdp/client/disp.h>
#include <freerdp/client/rdpsnd.h>
#include <freerdp/codec/color.h>
#include <freerdp/event.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/gdi/gfx.h>
#include <winpr/synch.h>
#include <winpr/thread.h>

#include <SDL2/SDL.h>

#include <cstring>

namespace gvrdp {

RdpSession::RdpSession() = default;

RdpSession::~RdpSession() {
    disconnect();
}

bool RdpSession::connect(const ConnectionProfile& profile, uint32_t sdl_window_id) {
    if (connected_) {
        LOG_WARN("Already connected, disconnect first");
        return false;
    }

    profile_ = profile;
    sdl_window_id_ = sdl_window_id;
    ignore_certificate_ = profile.ignore_certificate;
    last_error_ = RdpError::None;
    should_disconnect_ = false;

    // Create FreeRDP instance
    instance_ = freerdp_new();
    if (!instance_) {
        LOG_ERROR("Failed to create FreeRDP instance");
        last_error_ = RdpError::InternalError;
        return false;
    }

    // Set context size for our extended context
    instance_->ContextSize = sizeof(GvrdpContext);

    // Set callbacks before context creation
    instance_->PreConnect = gvrdp_pre_connect;
    instance_->PostConnect = gvrdp_post_connect;
    instance_->PostDisconnect = gvrdp_post_disconnect;
    instance_->VerifyCertificateEx = gvrdp_verify_certificate_ex;

    // Create context
    if (!freerdp_context_new(instance_)) {
        LOG_ERROR("Failed to create FreeRDP context");
        freerdp_free(instance_);
        instance_ = nullptr;
        last_error_ = RdpError::InternalError;
        return false;
    }

    // Set back-pointer
    context_ = reinterpret_cast<GvrdpContext*>(instance_->context);
    context_->session = this;

    // Apply connection profile settings
    rdpSettings* settings = instance_->context->settings;
    if (!apply_profile_to_settings(settings, profile_)) {
        LOG_ERROR("Failed to apply profile settings");
        freerdp_context_free(instance_);
        freerdp_free(instance_);
        instance_ = nullptr;
        context_ = nullptr;
        last_error_ = RdpError::InternalError;
        return false;
    }

    // Create display channel handler
    disp_channel_ = std::make_unique<DispChannel>();

    // Launch RDP thread
    rdp_thread_ = std::thread(&RdpSession::rdp_thread_func, this);

    return true;
}

void RdpSession::disconnect() {
    should_disconnect_ = true;

    if (instance_ && connected_) {
        freerdp_abort_connect_context(instance_->context);
    }

    if (rdp_thread_.joinable()) {
        rdp_thread_.join();
    }

    disp_channel_.reset();

    if (instance_) {
        freerdp_context_free(instance_);
        freerdp_free(instance_);
        instance_ = nullptr;
        context_ = nullptr;
    }

    connected_ = false;
}

bool RdpSession::is_connected() const {
    return connected_;
}

RdpError RdpSession::last_error() const {
    return last_error_;
}

void RdpSession::request_resolution_change(uint32_t width, uint32_t height) {
    if (disp_channel_) {
        disp_channel_->send_layout(width, height);
    }
}

void RdpSession::send_keyboard_event(uint16_t flags, uint8_t code) {
    if (!connected_ || !instance_ || !instance_->context) return;
    std::lock_guard lock(send_mutex_);
    freerdp_input_send_keyboard_event(instance_->context->input, flags, code);
}

void RdpSession::send_mouse_event(uint16_t flags, uint16_t x, uint16_t y) {
    if (!connected_ || !instance_ || !instance_->context) return;
    std::lock_guard lock(send_mutex_);
    freerdp_input_send_mouse_event(instance_->context->input, flags, x, y);
}

void RdpSession::send_extended_mouse_event(uint16_t flags, uint16_t x, uint16_t y) {
    if (!connected_ || !instance_ || !instance_->context) return;
    std::lock_guard lock(send_mutex_);
    freerdp_input_send_extended_mouse_event(instance_->context->input, flags, x, y);
}

const uint8_t* RdpSession::gdi_buffer() const {
    if (!instance_ || !instance_->context) return nullptr;
    rdpGdi* gdi = instance_->context->gdi;
    return gdi ? gdi->primary_buffer : nullptr;
}

uint32_t RdpSession::gdi_width() const {
    if (!instance_ || !instance_->context || !instance_->context->gdi) return 0;
    return static_cast<uint32_t>(instance_->context->gdi->width);
}

uint32_t RdpSession::gdi_height() const {
    if (!instance_ || !instance_->context || !instance_->context->gdi) return 0;
    return static_cast<uint32_t>(instance_->context->gdi->height);
}

uint32_t RdpSession::gdi_stride() const {
    if (!instance_ || !instance_->context || !instance_->context->gdi) return 0;
    return static_cast<uint32_t>(instance_->context->gdi->stride);
}

// ── Callbacks ──────────────────────────────────────────────────────────

bool RdpSession::on_pre_connect() {
    LOG_INFO("PreConnect callback");

    rdpSettings* settings = instance_->context->settings;

    // Load client addins (channels)
    if (!freerdp_client_load_addins(instance_->context->channels, settings)) {
        LOG_ERROR("Failed to load client addins");
        return false;
    }

    return true;
}

bool RdpSession::on_post_connect() {
    LOG_INFO("PostConnect callback");

    rdpContext* ctx = instance_->context;

    // Initialize GDI with BGRA32 format (matches SDL ARGB8888)
    if (!gdi_init(instance_, PIXEL_FORMAT_BGRA32)) {
        LOG_ERROR("Failed to initialize GDI");
        return false;
    }

    // Set update callbacks
    rdpUpdate* update = ctx->update;
    update->BeginPaint = gvrdp_begin_paint;
    update->EndPaint = gvrdp_end_paint;
    update->DesktopResize = gvrdp_desktop_resize;

    // Subscribe to channel connect/disconnect events
    PubSub_SubscribeChannelConnected(ctx->pubSub, gvrdp_on_channel_connected);
    PubSub_SubscribeChannelDisconnected(ctx->pubSub, gvrdp_on_channel_disconnected);

    // Register graphics pipeline if available
    if (!gdi_init_ex(instance_, PIXEL_FORMAT_BGRA32, 0, nullptr, nullptr)) {
        // Not fatal, gdi_init already succeeded
    }

    connected_ = true;

    // Push event to main thread
    push_sdl_event(GVRDP_EVENT_FRAME_READY);

    return true;
}

void RdpSession::on_post_disconnect() {
    LOG_INFO("PostDisconnect callback");

    if (instance_ && instance_->context) {
        PubSub_UnsubscribeChannelConnected(instance_->context->pubSub,
                                           gvrdp_on_channel_connected);
        PubSub_UnsubscribeChannelDisconnected(instance_->context->pubSub,
                                              gvrdp_on_channel_disconnected);
        gdi_free(instance_);
    }

    connected_ = false;
    push_sdl_event(GVRDP_EVENT_DISCONNECT);
}

bool RdpSession::on_begin_paint() {
    rdpGdi* gdi = instance_->context->gdi;
    gdi->primary->hdc->hwnd->invalid->null = TRUE;
    gdi->primary->hdc->hwnd->ninvalid = 0;
    return true;
}

bool RdpSession::on_end_paint() {
    rdpGdi* gdi = instance_->context->gdi;
    if (gdi->primary->hdc->hwnd->invalid->null) return true;

    // Push frame ready event to main thread
    push_sdl_event(GVRDP_EVENT_FRAME_READY);
    return true;
}

bool RdpSession::on_desktop_resize() {
    LOG_INFO("Desktop resize: {}x{}", gdi_width(), gdi_height());

    rdpGdi* gdi = instance_->context->gdi;
    rdpSettings* settings = instance_->context->settings;

    uint32_t width = freerdp_settings_get_uint32(settings, FreeRDP_DesktopWidth);
    uint32_t height = freerdp_settings_get_uint32(settings, FreeRDP_DesktopHeight);

    if (!gdi_resize(gdi, width, height)) {
        LOG_ERROR("gdi_resize failed");
        return false;
    }

    push_sdl_event(GVRDP_EVENT_RESIZE);
    return true;
}

uint32_t RdpSession::on_verify_certificate(const char* host, uint16_t port,
                                            const char* /*common_name*/, const char* subject,
                                            const char* issuer, const char* fingerprint,
                                            uint32_t /*flags*/) {
    LOG_WARN("Certificate verification for {}:{}", host ? host : "", port);
    LOG_WARN("  Subject: {}", subject ? subject : "");
    LOG_WARN("  Issuer: {}", issuer ? issuer : "");
    LOG_WARN("  Fingerprint: {}", fingerprint ? fingerprint : "");

    if (ignore_certificate_) {
        LOG_WARN("Auto-accepting certificate (ignore_certificate=true)");
        return 1;  // Accept permanently
    }

    // For now, accept temporarily (2). In future, show UI dialog.
    return 2;  // Accept temporarily
}

void RdpSession::on_channel_connected(const char* name, void* iface) {
    if (!name) return;

    if (strcmp(name, DISP_DVC_CHANNEL_NAME) == 0) {
        if (disp_channel_) {
            disp_channel_->on_connected(static_cast<DispClientContext*>(iface));
        }
    }
    // Additional channels handled here in future phases
}

void RdpSession::on_channel_disconnected(const char* name, void* /*iface*/) {
    if (!name) return;

    if (strcmp(name, DISP_DVC_CHANNEL_NAME) == 0) {
        if (disp_channel_) {
            disp_channel_->on_disconnected();
        }
    }
}

// ── RDP Thread ─────────────────────────────────────────────────────────

void RdpSession::rdp_thread_func() {
    LOG_INFO("RDP thread started");

    if (!freerdp_connect(instance_)) {
        LOG_ERROR("freerdp_connect failed");
        UINT32 error = freerdp_get_last_error(instance_->context);
        LOG_ERROR("FreeRDP error: 0x{:08X} - {}", error,
                  freerdp_get_last_error_string(error));
        last_error_ = RdpError::ConnectionFailed;
        push_sdl_event(GVRDP_EVENT_ERROR);
        return;
    }

    // Event loop
    while (!freerdp_shall_disconnect_context(instance_->context) && !should_disconnect_) {
        HANDLE handles[64] = {};
        DWORD nCount = freerdp_get_event_handles(instance_->context, handles, 64);
        if (nCount == 0) {
            LOG_ERROR("freerdp_get_event_handles failed");
            break;
        }

        DWORD status = WaitForMultipleObjects(nCount, handles, FALSE, 100);
        if (status == WAIT_FAILED) {
            LOG_ERROR("WaitForMultipleObjects failed");
            break;
        }

        if (!freerdp_check_event_handles(instance_->context)) {
            if (freerdp_get_last_error(instance_->context) ==
                FREERDP_ERROR_SUCCESS) {
                continue;
            }
            LOG_ERROR("freerdp_check_event_handles failed");
            break;
        }
    }

    freerdp_disconnect(instance_);
    LOG_INFO("RDP thread finished");
}

void RdpSession::push_sdl_event(GvrdpEvent type, int /*code*/, void* data1) {
    SDL_Event event = {};
    event.type = SDL_USEREVENT;
    event.user.windowID = sdl_window_id_;
    event.user.code = static_cast<int>(type);
    event.user.data1 = data1;
    event.user.data2 = nullptr;
    SDL_PushEvent(&event);
}

}  // namespace gvrdp
