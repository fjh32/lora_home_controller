#include "lora_engine.h"
#include "lora_codec.h"
#include <string.h>

void lora_engine_init(LoraEngine *engine, LoraDriver *driver)
{
    memset(engine, 0, sizeof(*engine));
    engine->driver = driver;
}

uint8_t lora_engine_send(LoraEngine *engine,
                         LoraMessage *msg,
                         uint16_t timeout)
{
    if (!engine || !msg || !engine->driver->transmit) {
        return 0;
    }

    if (msg->metadata.source == 0) {
        msg->metadata.source = engine->driver->local_id;
    }

    uint8_t buf[LORA_MAX_ENCODED_SIZE];
    size_t len = lora_encode(msg, buf, sizeof(buf));
    if (len == 0 || len > 255) {
        return 0;
    }

    return engine->driver->transmit(engine->driver->lora_ctx,
                                   buf,
                                   (uint8_t)len,
                                   timeout);
}

void lora_engine_handle_message(LoraEngine *engine,
                                const LoraMessage *msg)
{
    if (!engine || !msg) return;

    const LoraMetadata *meta = &msg->metadata;

    // Routing done here, if dest matches my local_id or a broadcast, I want to handle it.
    if(meta->dest == engine->local_id || meta->dest == LORA_NODE_BROADCAST_ID) {

        switch (msg->message_type) {
            case LORA_PING_REQUEST:
                if (engine->on_ping_req) {
                    engine->on_ping_req(engine, &msg->payload.ping_req, meta);
                }
                break;

            case LORA_PING_RESPONSE:
                if (engine->on_ping_resp) {
                    engine->on_ping_resp(engine, &msg->payload.ping_resp, meta);
                }
                break;

            case LORA_DATA_REQUEST:
                if (engine->on_data_req) {
                    engine->on_data_req(engine, &msg->payload.data_req, meta);
                }
                break;

            case LORA_DATA:
                if (engine->on_data) {
                    engine->on_data(engine, &msg->payload.data, meta);
                }
                break;

            case LORA_COMMAND_REQUEST:
                if (engine->on_command_req) {
                    engine->on_command_req(engine, &msg->payload.command_req, meta);
                }
                break;

            case LORA_COMMAND_RESPONSE:
                if (engine->on_command_resp) {
                    engine->on_command_resp(engine, &msg->payload.command_resp, meta);
                }
                break;

            case LORA_STREAM_REQUEST:
                if (engine->on_stream_req) {
                    engine->on_stream_req(engine, &msg->payload.stream_req, meta);
                }
                break;

            case LORA_STREAM_ANNOUNCE:
                if (engine->on_stream_announce) {
                    engine->on_stream_announce(engine, &msg->payload.stream_announce, meta);
                }
                break;

            case LORA_STREAM_ANNOUNCE_ACK:
                if (engine->on_stream_announce_ack) {
                    engine->on_stream_announce_ack(engine, &msg->payload.stream_announce_ack, meta);
                }
                break;

            case LORA_STREAM_SEQUENCE:
                if (engine->on_stream_sequence) {
                    engine->on_stream_sequence(engine, &msg->payload.stream_sequence, meta);
                }
                break;

            case LORA_STREAM_SEQUENCE_ACK:
                if (engine->on_stream_seq_ack) {
                    engine->on_stream_seq_ack(engine, &msg->payload.stream_seq_ack, meta);
                }
                break;

            case LORA_STREAM_COMPLETE:
                if (engine->on_stream_complete) {
                    engine->on_stream_complete(engine, &msg->payload.stream_complete, meta);
                }
                break;

            case LORA_RAW:
            default:
                // ignore or extend later
                break;
        }
    }
}

void lora_engine_loop(LoraEngine *engine)
{
    while(1)
    {
        if(engine->driver->receive_ready_flag)
        {
            engine->driver->receive_ready_flag = 0;
            uint8_t received_data[LORA_MAX_ENCODED_SIZE];
            engine->driver->receive(engine->driver->lora_ctx, received_data, sizeof(received_data));

            LoraMessage msg;
            if(!lora_decode(received_data, sizeof(received_data), &msg))
            {
                lora_engine_handle_message(engine, &msg);
            }
            else 
            {
                // could not decode message - handle or ignore
            }
        }
    }
}

// Simple Requests
uint8_t lora_engine_send_ping(LoraEngine *engine,
                              NodeId dest,
                              uint16_t timeout)
{
    LoraMessage msg = {0};
    msg.message_type   = LORA_PING_REQUEST;
    msg.metadata.dest  = dest;
    return lora_engine_send(engine, &msg, timeout);
}