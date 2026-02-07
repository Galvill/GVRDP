#pragma once

#include "channels/channel_interface.hpp"

#include <freerdp/client/cliprdr.h>

#include <cstdint>
#include <string>
#include <vector>

namespace gvrdp {

// Clipboard Redirection channel — synchronizes clipboard between local and remote.
class CliprdrChannel : public ChannelInterface {
public:
    CliprdrChannel();
    ~CliprdrChannel() override;

    std::string channel_name() const override;
    bool is_connected() const override;

    void on_connected(CliprdrClientContext* cliprdr_ctx);
    void on_disconnected();

    // Send local clipboard text to remote
    void send_text(const std::string& text);

    // Get last received text from remote
    std::string received_text() const;

private:
    // FreeRDP callback handlers
    static UINT on_monitor_ready(CliprdrClientContext* context,
                                 const CLIPRDR_MONITOR_READY* monitor_ready);
    static UINT on_server_capabilities(CliprdrClientContext* context,
                                       const CLIPRDR_CAPABILITIES* capabilities);
    static UINT on_server_format_list(CliprdrClientContext* context,
                                      const CLIPRDR_FORMAT_LIST* format_list);
    static UINT on_server_format_list_response(CliprdrClientContext* context,
                                               const CLIPRDR_FORMAT_LIST_RESPONSE* response);
    static UINT on_server_format_data_request(CliprdrClientContext* context,
                                              const CLIPRDR_FORMAT_DATA_REQUEST* request);
    static UINT on_server_format_data_response(CliprdrClientContext* context,
                                               const CLIPRDR_FORMAT_DATA_RESPONSE* response);

    CliprdrClientContext* cliprdr_ctx_ = nullptr;
    std::string received_text_;
    std::string local_text_;
    bool has_text_format_ = false;
    uint32_t text_format_id_ = 0;
};

}  // namespace gvrdp
