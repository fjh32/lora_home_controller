#include "lora_engine.h"

// decodes raw bytes into a LoraMessage (type + payload)
// put into lora_codec.h/c
int lora_decode_message(const uint8_t *buf, size_t len, LoraMessage *out_msg, LoraMeta *out_meta);

void lora_engine_process_rx(const uint8_t *buf, size_t len,
                            const LoraHandlers *handlers)
{
    LoraMessageType msg;
    LoraMeta meta;

    if (lora_decode_message(buf, len, &msg, &meta) != 0) {
        // invalid packet, drop
        return;
    }

    switch (msg) {
    case LORA_PING_REQUEST:
        if (handlers->on_ping_req) {
            handlers->on_ping_req(&msg.payload.ping_req, &meta, handlers->ctx);
        }
        break;

    case LORA_PING_RESPONSE:
        if (handlers->on_ping_resp) {
            handlers->on_ping_resp(&msg.payload.ping_resp, &meta, handlers->ctx);
        }
        break;

    case LORA_DATA_REQUEST:
        if (handlers->on_data_req) {
            handlers->on_data_req(&msg.payload.data_req, &meta, handlers->ctx);
        }
        break;

    case LORA_DATA_RESPONSE:
        if (handlers->on_data_resp) {
            handlers->on_data_resp(&msg.payload.data_resp, &meta, handlers->ctx);
        }
        break;

    case LORA_COMMAND_REQUEST:
        if (handlers->on_command_req) {
            handlers->on_command_req(&msg.payload.command_req, &meta, handlers->ctx);
        }
        break;

    case LORA_COMMAND_RESPONSE:
        if (handlers->on_command_resp) {
            handlers->on_command_resp(&msg.payload.command_resp, &meta, handlers->ctx);
        }
        break;
    }
}
