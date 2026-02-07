#include "channels/cliprdr_channel.hpp"

#include "util/logger.hpp"

#include <freerdp/channels/cliprdr.h>

#include <cstring>

namespace gvrdp {

CliprdrChannel::CliprdrChannel() = default;
CliprdrChannel::~CliprdrChannel() = default;

std::string CliprdrChannel::channel_name() const {
    return CLIPRDR_SVC_CHANNEL_NAME;
}

bool CliprdrChannel::is_connected() const {
    return cliprdr_ctx_ != nullptr;
}

void CliprdrChannel::on_connected(CliprdrClientContext* cliprdr_ctx) {
    cliprdr_ctx_ = cliprdr_ctx;
    if (!cliprdr_ctx_) return;

    // Store back-pointer
    cliprdr_ctx_->custom = this;

    // Set callback handlers
    cliprdr_ctx_->MonitorReady = on_monitor_ready;
    cliprdr_ctx_->ServerCapabilities = on_server_capabilities;
    cliprdr_ctx_->ServerFormatList = on_server_format_list;
    cliprdr_ctx_->ServerFormatListResponse = on_server_format_list_response;
    cliprdr_ctx_->ServerFormatDataRequest = on_server_format_data_request;
    cliprdr_ctx_->ServerFormatDataResponse = on_server_format_data_response;

    LOG_INFO("Clipboard channel connected");
}

void CliprdrChannel::on_disconnected() {
    cliprdr_ctx_ = nullptr;
    LOG_INFO("Clipboard channel disconnected");
}

void CliprdrChannel::send_text(const std::string& text) {
    if (!cliprdr_ctx_ || !cliprdr_ctx_->ClientFormatList) return;

    local_text_ = text;

    // Announce CF_UNICODETEXT format
    CLIPRDR_FORMAT formats[1] = {};
    formats[0].formatId = CF_UNICODETEXT;
    formats[0].formatName = nullptr;

    CLIPRDR_FORMAT_LIST format_list = {};
    format_list.common.msgType = CB_FORMAT_LIST;
    format_list.numFormats = 1;
    format_list.formats = formats;

    cliprdr_ctx_->ClientFormatList(cliprdr_ctx_, &format_list);
}

std::string CliprdrChannel::received_text() const {
    return received_text_;
}

UINT CliprdrChannel::on_monitor_ready(CliprdrClientContext* context,
                                       const CLIPRDR_MONITOR_READY* /*monitor_ready*/) {
    LOG_DEBUG("Clipboard monitor ready");

    // Send capabilities
    CLIPRDR_GENERAL_CAPABILITY_SET general_caps = {};
    general_caps.capabilitySetType = CB_CAPSTYPE_GENERAL;
    general_caps.capabilitySetLength = 12;
    general_caps.version = CB_CAPS_VERSION_2;
    general_caps.generalFlags = CB_USE_LONG_FORMAT_NAMES;

    CLIPRDR_CAPABILITIES caps = {};
    caps.common.msgType = CB_CLIP_CAPS;
    caps.cCapabilitiesSets = 1;
    caps.capabilitySets = reinterpret_cast<CLIPRDR_CAPABILITY_SET*>(&general_caps);

    if (context->ClientCapabilities) {
        context->ClientCapabilities(context, &caps);
    }

    return CHANNEL_RC_OK;
}

UINT CliprdrChannel::on_server_capabilities(CliprdrClientContext* /*context*/,
                                             const CLIPRDR_CAPABILITIES* /*capabilities*/) {
    LOG_DEBUG("Server clipboard capabilities received");
    return CHANNEL_RC_OK;
}

UINT CliprdrChannel::on_server_format_list(CliprdrClientContext* context,
                                            const CLIPRDR_FORMAT_LIST* format_list) {
    auto* self = static_cast<CliprdrChannel*>(context->custom);
    if (!self) return ERROR_INTERNAL_ERROR;

    self->has_text_format_ = false;

    // Look for text formats
    for (UINT32 i = 0; i < format_list->numFormats; i++) {
        if (format_list->formats[i].formatId == CF_UNICODETEXT ||
            format_list->formats[i].formatId == CF_TEXT) {
            self->has_text_format_ = true;
            self->text_format_id_ = format_list->formats[i].formatId;
            break;
        }
    }

    // Send response
    CLIPRDR_FORMAT_LIST_RESPONSE response = {};
    response.common.msgType = CB_FORMAT_LIST_RESPONSE;
    response.common.msgFlags = CB_RESPONSE_OK;
    if (context->ClientFormatListResponse) {
        context->ClientFormatListResponse(context, &response);
    }

    // Request text data if available
    if (self->has_text_format_ && context->ClientFormatDataRequest) {
        CLIPRDR_FORMAT_DATA_REQUEST request = {};
        request.common.msgType = CB_FORMAT_DATA_REQUEST;
        request.requestedFormatId = self->text_format_id_;
        context->ClientFormatDataRequest(context, &request);
    }

    return CHANNEL_RC_OK;
}

UINT CliprdrChannel::on_server_format_list_response(
    CliprdrClientContext* /*context*/, const CLIPRDR_FORMAT_LIST_RESPONSE* /*response*/) {
    return CHANNEL_RC_OK;
}

UINT CliprdrChannel::on_server_format_data_request(CliprdrClientContext* context,
                                                    const CLIPRDR_FORMAT_DATA_REQUEST* request) {
    auto* self = static_cast<CliprdrChannel*>(context->custom);
    if (!self) return ERROR_INTERNAL_ERROR;

    CLIPRDR_FORMAT_DATA_RESPONSE response = {};
    response.common.msgType = CB_FORMAT_DATA_RESPONSE;
    response.common.msgFlags = CB_RESPONSE_OK;

    if (request->requestedFormatId == CF_UNICODETEXT && !self->local_text_.empty()) {
        // Convert UTF-8 to UTF-16LE
        // Simple conversion for ASCII range; full Unicode would need proper conversion
        std::vector<uint8_t> utf16;
        for (char c : self->local_text_) {
            utf16.push_back(static_cast<uint8_t>(c));
            utf16.push_back(0);
        }
        utf16.push_back(0);  // null terminator
        utf16.push_back(0);

        response.requestedFormatData = utf16.data();
        response.common.dataLen = static_cast<UINT32>(utf16.size());

        if (context->ClientFormatDataResponse) {
            return context->ClientFormatDataResponse(context, &response);
        }
    } else {
        response.common.msgFlags = CB_RESPONSE_FAIL;
        if (context->ClientFormatDataResponse) {
            return context->ClientFormatDataResponse(context, &response);
        }
    }

    return CHANNEL_RC_OK;
}

UINT CliprdrChannel::on_server_format_data_response(
    CliprdrClientContext* context, const CLIPRDR_FORMAT_DATA_RESPONSE* response) {
    auto* self = static_cast<CliprdrChannel*>(context->custom);
    if (!self) return ERROR_INTERNAL_ERROR;

    if (response->common.msgFlags != CB_RESPONSE_OK || !response->requestedFormatData) {
        return CHANNEL_RC_OK;
    }

    // Convert UTF-16LE to UTF-8 (simple ASCII conversion)
    self->received_text_.clear();
    const auto* data = reinterpret_cast<const uint16_t*>(response->requestedFormatData);
    size_t len = response->common.dataLen / 2;
    for (size_t i = 0; i < len && data[i] != 0; i++) {
        if (data[i] < 128) {
            self->received_text_ += static_cast<char>(data[i]);
        }
    }

    LOG_DEBUG("Received clipboard text ({} chars)", self->received_text_.size());
    return CHANNEL_RC_OK;
}

}  // namespace gvrdp
