#include "channels/rdpsnd_channel.hpp"

#include "util/logger.hpp"

namespace gvrdp {

RdpsndChannel::RdpsndChannel() = default;
RdpsndChannel::~RdpsndChannel() = default;

std::string RdpsndChannel::channel_name() const {
    return "rdpsnd";
}

bool RdpsndChannel::is_connected() const {
    return connected_;
}

void RdpsndChannel::on_connected(void* /*rdpsnd_ctx*/) {
    connected_ = true;
    LOG_INFO("Audio channel connected (playback via built-in FreeRDP rdpsnd plugin)");
}

void RdpsndChannel::on_disconnected() {
    connected_ = false;
    LOG_INFO("Audio channel disconnected");
}

}  // namespace gvrdp
