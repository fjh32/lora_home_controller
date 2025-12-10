#include "lora_engine.h"
#include <string.h>

void lora_engine_init(LoraEngine *engine,
                      const LoraDriver *driver,
                      const LoraHandlers *handlers)
{
    memset(engine, 0, sizeof(*engine));
    engine->driver   = *driver;
    engine->handlers = *handlers;
}

/**
 * Internal: encode & transmit using driver.
 */
uint8_t lora_engine_send(LoraEngine *engine,
                         LoraMessage *msg,
                         uint16_t timeout)
{
    if (!engine || !msg || !engine->driver.transmit) {
        return 0;
    }

    // Default source if not set
    if (msg->metadata.source == 0) {
        msg->metadata.source = engine->driver.local_id;
    }

    uint8_t buf[LORA_MAX_ENCODED_SIZE];
    size_t len = lora_encode(msg, buf, sizeof(buf));
    if (len == 0) {
        return 0;
    }

    if (len > 255) {
        // your driver only takes uint8_t length; be safe
        return 0;
    }

    return engine->driver.transmit(
                                   buf,
                                   (uint8_t)len,
                                   timeout);
}

uint8_t lora_engine_send_ping(LoraEngine *engine,
                              NodeId dest,
                              uint16_t timeout)
{
    LoraMessage msg = {0};
    msg.message_type     = LORA_PING_REQUEST;
    msg.metadata.source  = engine->driver.local_id; // optional, send() will fill
    msg.metadata.dest    = dest;
    return lora_engine_send(engine, &msg, timeout);
}


void lora_engine_handle_message(LoraEngine *engine,
                                const LoraMessage *msg)
{
    if (!engine || !msg) {
        return;
    }

    const LoraMetadata *meta = &msg->metadata;
    void *ctx = engine->handlers.ctx;

    switch (msg->message_type) {
        case LORA_PING_REQUEST:
            if (engine->handlers.on_ping_req) {
                engine->handlers.on_ping_req(&msg->payload.ping_req, meta, ctx);
            }
            break;

        case LORA_PING_RESPONSE:
            if (engine->handlers.on_ping_resp) {
                engine->handlers.on_ping_resp(&msg->payload.ping_resp, meta, ctx);
            }
            break;

        case LORA_DATA_REQUEST:
            if (engine->handlers.on_data_req) {
                engine->handlers.on_data_req(&msg->payload.data_req, meta, ctx);
            }
            break;

        case LORA_DATA:
            if (engine->handlers.on_data) {
                engine->handlers.on_data(&msg->payload.data, meta, ctx);
            }
            break;

        case LORA_COMMAND_REQUEST:
            if (engine->handlers.on_command_req) {
                engine->handlers.on_command_req(&msg->payload.command_req, meta, ctx);
            }
            break;

        case LORA_COMMAND_RESPONSE:
            if (engine->handlers.on_command_resp) {
                engine->handlers.on_command_resp(&msg->payload.command_resp, meta, ctx);
            }
            break;

        case LORA_STREAM_REQUEST:
            if (engine->handlers.on_stream_req) {
                engine->handlers.on_stream_req(&msg->payload.stream_req, meta, ctx);
            }
            break;

        case LORA_STREAM_ANNOUNCE:
            if (engine->handlers.on_stream_announce) {
                engine->handlers.on_stream_announce(&msg->payload.stream_announce,
                                                    meta, ctx);
            }
            break;

        case LORA_STREAM_ANNOUNCE_ACK:
            if (engine->handlers.on_stream_announce_ack) {
                engine->handlers.on_stream_announce_ack(&msg->payload.stream_announce_ack,
                                                        meta, ctx);
            }
            break;

        case LORA_STREAM_SEQUENCE:
            if (engine->handlers.on_stream_sequence) {
                engine->handlers.on_stream_sequence(&msg->payload.stream_sequence,
                                                    meta, ctx);
            }
            break;

        case LORA_STREAM_SEQUENCE_ACK:
            if (engine->handlers.on_stream_seq_ack) {
                engine->handlers.on_stream_seq_ack(&msg->payload.stream_seq_ack,
                                                meta, ctx);
            }
            break;

        case LORA_STREAM_COMPLETE:
            if (engine->handlers.on_stream_complete) {
                engine->handlers.on_stream_complete(&msg->payload.stream_complete,
                                                    meta, ctx);
            }
            break;

        case LORA_RAW:
            // TODO
            break;

        default:
            // invalid
            break;
    }
}
