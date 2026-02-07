#pragma once

#include "channels/channel_interface.hpp"

#include <freerdp/client/disp.h>

#include <chrono>
#include <cstdint>

namespace gvrdp {

// Display Control Virtual Channel — sends resolution change requests
// to the server via the DISP DVC (MS-RDPEDISP).
class DispChannel : public ChannelInterface {
public:
    DispChannel();
    ~DispChannel() override = default;

    std::string channel_name() const override;
    bool is_connected() const override;

    void on_connected(DispClientContext* disp_ctx);
    void on_disconnected();

    // Send a single-monitor layout with the given resolution.
    // Has a built-in 200ms minimum interval guard.
    bool send_layout(uint32_t width, uint32_t height);

private:
    DispClientContext* disp_ctx_ = nullptr;
    std::chrono::steady_clock::time_point last_send_;
    static constexpr auto kMinInterval = std::chrono::milliseconds(200);
};

}  // namespace gvrdp
