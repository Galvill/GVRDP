#include "channels/rdpdr_channel.hpp"

#include "util/logger.hpp"

namespace gvrdp {

RdpdrChannel::RdpdrChannel() = default;
RdpdrChannel::~RdpdrChannel() = default;

std::string RdpdrChannel::channel_name() const {
    return "rdpdr";
}

bool RdpdrChannel::is_connected() const {
    return connected_;
}

void RdpdrChannel::on_connected(void* /*rdpdr_ctx*/) {
    connected_ = true;
    LOG_INFO("Drive redirection channel connected");
}

void RdpdrChannel::on_disconnected() {
    connected_ = false;
    LOG_INFO("Drive redirection channel disconnected");
}

}  // namespace gvrdp
