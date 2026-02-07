#include "core/rdp_callbacks.hpp"

#include "core/rdp_context.hpp"
#include "core/rdp_session.hpp"
#include "util/logger.hpp"

using namespace gvrdp;

static RdpSession* get_session(freerdp* instance) {
    if (!instance || !instance->context) return nullptr;
    return reinterpret_cast<GvrdpContext*>(instance->context)->session;
}

static RdpSession* get_session(rdpContext* context) {
    if (!context) return nullptr;
    return reinterpret_cast<GvrdpContext*>(context)->session;
}

extern "C" {

BOOL gvrdp_pre_connect(freerdp* instance) {
    auto* session = get_session(instance);
    if (!session) return FALSE;
    return session->on_pre_connect() ? TRUE : FALSE;
}

BOOL gvrdp_post_connect(freerdp* instance) {
    auto* session = get_session(instance);
    if (!session) return FALSE;
    return session->on_post_connect() ? TRUE : FALSE;
}

void gvrdp_post_disconnect(freerdp* instance) {
    auto* session = get_session(instance);
    if (session) session->on_post_disconnect();
}

BOOL gvrdp_begin_paint(rdpContext* context) {
    auto* session = get_session(context);
    if (!session) return FALSE;
    return session->on_begin_paint() ? TRUE : FALSE;
}

BOOL gvrdp_end_paint(rdpContext* context) {
    auto* session = get_session(context);
    if (!session) return FALSE;
    return session->on_end_paint() ? TRUE : FALSE;
}

BOOL gvrdp_desktop_resize(rdpContext* context) {
    auto* session = get_session(context);
    if (!session) return FALSE;
    return session->on_desktop_resize() ? TRUE : FALSE;
}

DWORD gvrdp_verify_certificate_ex(freerdp* instance, const char* host, UINT16 port,
                                   const char* common_name, const char* subject,
                                   const char* issuer, const char* fingerprint, DWORD flags) {
    auto* session = get_session(instance);
    if (!session) return 0;
    return session->on_verify_certificate(host, port, common_name, subject, issuer, fingerprint,
                                          flags);
}

}  // extern "C"
