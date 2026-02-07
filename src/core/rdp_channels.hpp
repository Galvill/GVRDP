#pragma once

#include <freerdp/freerdp.h>

// PubSub event handlers for channel connect/disconnect.
// These are registered during PostConnect.

extern "C" {

void gvrdp_on_channel_connected(void* context, const ChannelConnectedEventArgs* e);
void gvrdp_on_channel_disconnected(void* context, const ChannelDisconnectedEventArgs* e);

}  // extern "C"
