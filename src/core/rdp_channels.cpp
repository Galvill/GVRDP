#include "core/rdp_channels.hpp"

#include "core/rdp_context.hpp"
#include "core/rdp_session.hpp"
#include "util/logger.hpp"

#include <freerdp/event.h>

using namespace gvrdp;

extern "C" {

void gvrdp_on_channel_connected(void* context, const ChannelConnectedEventArgs* e) {
    auto* ctx = reinterpret_cast<rdpContext*>(context);
    auto* session = reinterpret_cast<GvrdpContext*>(ctx)->session;
    if (session && e) {
        LOG_INFO("Channel connected: {}", e->name);
        session->on_channel_connected(e->name, e->pInterface);
    }
}

void gvrdp_on_channel_disconnected(void* context, const ChannelDisconnectedEventArgs* e) {
    auto* ctx = reinterpret_cast<rdpContext*>(context);
    auto* session = reinterpret_cast<GvrdpContext*>(ctx)->session;
    if (session && e) {
        LOG_INFO("Channel disconnected: {}", e->name);
        session->on_channel_disconnected(e->name, e->pInterface);
    }
}

}  // extern "C"
