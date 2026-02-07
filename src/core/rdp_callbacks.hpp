#pragma once

#include <freerdp/freerdp.h>
#include <freerdp/update.h>

// Static C trampolines that bridge FreeRDP C callbacks to RdpSession C++ methods.
// These recover the C++ object via the GvrdpContext back-pointer.

extern "C" {

BOOL gvrdp_pre_connect(freerdp* instance);
BOOL gvrdp_post_connect(freerdp* instance);
void gvrdp_post_disconnect(freerdp* instance);
BOOL gvrdp_begin_paint(rdpContext* context);
BOOL gvrdp_end_paint(rdpContext* context);
BOOL gvrdp_desktop_resize(rdpContext* context);
DWORD gvrdp_verify_certificate_ex(freerdp* instance, const char* host, UINT16 port,
                                   const char* common_name, const char* subject,
                                   const char* issuer, const char* fingerprint, DWORD flags);

}  // extern "C"
