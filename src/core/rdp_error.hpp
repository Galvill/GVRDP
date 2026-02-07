#pragma once

#include <cstdint>
#include <string>

namespace gvrdp {

enum class RdpError : uint32_t {
    None = 0,
    ConnectionFailed,
    AuthenticationFailed,
    CertificateRejected,
    NetworkError,
    DisconnectedByServer,
    DisconnectedByUser,
    ProtocolError,
    InternalError,
};

inline std::string rdp_error_to_string(RdpError error) {
    switch (error) {
        case RdpError::None: return "No error";
        case RdpError::ConnectionFailed: return "Connection failed";
        case RdpError::AuthenticationFailed: return "Authentication failed";
        case RdpError::CertificateRejected: return "Certificate rejected";
        case RdpError::NetworkError: return "Network error";
        case RdpError::DisconnectedByServer: return "Disconnected by server";
        case RdpError::DisconnectedByUser: return "Disconnected by user";
        case RdpError::ProtocolError: return "Protocol error";
        case RdpError::InternalError: return "Internal error";
    }
    return "Unknown error";
}

}  // namespace gvrdp
