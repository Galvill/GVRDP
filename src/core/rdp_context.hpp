#pragma once

#include <freerdp/client.h>
#include <freerdp/freerdp.h>

namespace gvrdp {

class RdpSession;

// Custom context extending rdpClientContext.
// MUST have rdpClientContext as first member for C-style inheritance.
struct GvrdpContext {
    rdpClientContext common;  // Must be first
    RdpSession* session;     // Back-pointer to C++ session object
};

}  // namespace gvrdp
