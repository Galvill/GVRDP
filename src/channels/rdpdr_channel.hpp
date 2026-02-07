#pragma once

#include "channels/channel_interface.hpp"

#include <string>

namespace gvrdp {

// Drive Redirection channel — shares a local folder with the remote session.
// Phase 7 implementation: register drive device, handle I/O.
class RdpdrChannel : public ChannelInterface {
public:
    RdpdrChannel();
    ~RdpdrChannel() override;

    std::string channel_name() const override;
    bool is_connected() const override;

    void on_connected(void* rdpdr_ctx);
    void on_disconnected();

private:
    bool connected_ = false;
};

}  // namespace gvrdp
