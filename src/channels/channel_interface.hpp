#pragma once

#include <string>

namespace gvrdp {

// Base interface for RDP channel handlers.
class ChannelInterface {
public:
    virtual ~ChannelInterface() = default;
    virtual std::string channel_name() const = 0;
    virtual bool is_connected() const = 0;
};

}  // namespace gvrdp
