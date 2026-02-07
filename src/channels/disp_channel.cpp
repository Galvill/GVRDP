#include "channels/disp_channel.hpp"

#include "util/logger.hpp"

#include <freerdp/channels/disp.h>

#include <algorithm>
#include <cstring>

namespace gvrdp {

DispChannel::DispChannel() : last_send_(std::chrono::steady_clock::now() - kMinInterval) {}

std::string DispChannel::channel_name() const {
    return DISP_DVC_CHANNEL_NAME;
}

bool DispChannel::is_connected() const {
    return disp_ctx_ != nullptr;
}

void DispChannel::on_connected(DispClientContext* disp_ctx) {
    disp_ctx_ = disp_ctx;
    LOG_INFO("Display control channel connected");
}

void DispChannel::on_disconnected() {
    disp_ctx_ = nullptr;
    LOG_INFO("Display control channel disconnected");
}

bool DispChannel::send_layout(uint32_t width, uint32_t height) {
    if (!disp_ctx_ || !disp_ctx_->SendMonitorLayout) {
        LOG_WARN("DISP channel not available");
        return false;
    }

    // Enforce minimum interval
    auto now = std::chrono::steady_clock::now();
    if (now - last_send_ < kMinInterval) {
        return false;
    }
    last_send_ = now;

    // Clamp dimensions
    width = std::clamp(width, static_cast<uint32_t>(DISPLAY_CONTROL_MIN_MONITOR_WIDTH),
                       static_cast<uint32_t>(DISPLAY_CONTROL_MAX_MONITOR_WIDTH));
    height = std::clamp(height, static_cast<uint32_t>(DISPLAY_CONTROL_MIN_MONITOR_HEIGHT),
                        static_cast<uint32_t>(DISPLAY_CONTROL_MAX_MONITOR_HEIGHT));

    // Ensure even dimensions (required by some RDP servers)
    width &= ~1u;
    height &= ~1u;

    DISPLAY_CONTROL_MONITOR_LAYOUT layout = {};
    layout.Flags = DISPLAY_CONTROL_MONITOR_PRIMARY;
    layout.Left = 0;
    layout.Top = 0;
    layout.Width = width;
    layout.Height = height;
    layout.PhysicalWidth = 0;
    layout.PhysicalHeight = 0;
    layout.Orientation = ORIENTATION_LANDSCAPE;
    layout.DesktopScaleFactor = 100;
    layout.DeviceScaleFactor = 100;

    UINT result = disp_ctx_->SendMonitorLayout(disp_ctx_, 1, &layout);
    if (result != CHANNEL_RC_OK) {
        LOG_ERROR("SendMonitorLayout failed: 0x{:08X}", result);
        return false;
    }

    LOG_INFO("Sent display layout: {}x{}", width, height);
    return true;
}

}  // namespace gvrdp
