#pragma once

#include "channels/channel_interface.hpp"

#include <cstdint>
#include <string>

namespace gvrdp {

// Audio Redirection channel — plays remote audio locally via SDL2 audio.
// Phase 5 implementation: format negotiation + SDL_OpenAudioDevice playback.
class RdpsndChannel : public ChannelInterface {
public:
    RdpsndChannel();
    ~RdpsndChannel() override;

    std::string channel_name() const override;
    bool is_connected() const override;

    void on_connected(void* rdpsnd_ctx);
    void on_disconnected();

private:
    bool connected_ = false;
};

}  // namespace gvrdp
