#pragma once

#include <stdint.h>
#include "lora_message_types.h"
#include "lora_codec.h"

typedef void (*LoraPingReqHandler)(const LoraPingReq *msg,
                                   const LoraMetadata *meta,
                                   void *ctx);

typedef void (*LoraPingRespHandler)(const LoraPingResp *msg,
                                    const LoraMetadata *meta,
                                    void *ctx);

typedef void (*LoraDataReqHandler)(const LoraDataReq *msg,
                                   const LoraMetadata *meta,
                                   void *ctx);

typedef void (*LoraDataHandler)(const LoraData *msg,
                                const LoraMetadata *meta,
                                void *ctx);

typedef void (*LoraCommandReqHandler)(const LoraCommandReq *msg,
                                      const LoraMetadata *meta,
                                      void *ctx);

typedef void (*LoraCommandRespHandler)(const LoraCommandResp *msg,
                                       const LoraMetadata *meta,
                                       void *ctx);

typedef void (*LoraStreamRequestHandler)(const LoraStreamRequest *msg,
                                         const LoraMetadata *meta,
                                         void *ctx);

typedef void (*LoraStreamAnnounceHandler)(const LoraStreamAnnounce *msg,
                                          const LoraMetadata *meta,
                                          void *ctx);

typedef void (*LoraStreamAnnounceAckHandler)(const LoraStreamAnnounceAck *msg,
                                             const LoraMetadata *meta,
                                             void *ctx);

typedef void (*LoraStreamSequenceHandler)(const LoraStreamSequence *msg,
                                          const LoraMetadata *meta,
                                          void *ctx);

typedef void (*LoraStreamSequenceAckHandler)(const LoraStreamSequenceAck *msg,
                                             const LoraMetadata *meta,
                                             void *ctx);

typedef void (*LoraStreamCompleteHandler)(const LoraStreamComplete *msg,
                                          const LoraMetadata *meta,
                                          void *ctx);

typedef struct {
    LoraPingReqHandler           on_ping_req;
    LoraPingRespHandler          on_ping_resp;

    LoraDataReqHandler           on_data_req;
    LoraDataHandler              on_data;

    LoraCommandReqHandler        on_command_req;
    LoraCommandRespHandler       on_command_resp;

    LoraStreamRequestHandler     on_stream_req;
    LoraStreamAnnounceHandler    on_stream_announce;
    LoraStreamAnnounceAckHandler on_stream_announce_ack;
    LoraStreamSequenceHandler    on_stream_sequence;
    LoraStreamSequenceAckHandler on_stream_seq_ack;
    LoraStreamCompleteHandler    on_stream_complete;

    void *ctx;
} LoraHandlers;

typedef struct {
    NodeId local_id;
    uint8_t (*transmit)(uint8_t* data, uint8_t length, uint16_t timeout);
    void * lora_ctx;
} LoraDriver;

typedef struct {
    NodeId local_id;
    LoraHandlers handlers;
    LoraDriver driver;
} LoraEngine;

void lora_engine_init(LoraEngine *engine,
                      const LoraDriver *driver,
                      const LoraHandlers *handlers);

/**
 * Encode and transmit a message using the engine's driver.
 * - Fills msg->metadata.source with driver.local_id if it is 0.
 *
 * @return driver transmit return value (0/1 or whatever your driver uses),
 *         or 0 on encode error / missing transmit function.
 */
uint8_t lora_engine_send(LoraEngine *engine,
                         LoraMessage *msg,
                         uint16_t timeout);

/**
 * Convenience helper: send a PING_REQUEST to a destination node.
 */
uint8_t lora_engine_send_ping(LoraEngine *engine,
                              NodeId dest,
                              uint16_t timeout);

/**
 * Dispatch a decoded message to appropriate handler.
 */
void lora_engine_handle_message(LoraEngine *engine,
                                const LoraMessage *msg);
