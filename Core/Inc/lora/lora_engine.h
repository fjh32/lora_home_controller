#pragma once

#include <stdint.h>
#include "lora_message_types.h"

typedef struct _LoraEngine LoraEngine;

typedef struct {
    NodeId local_id;
    uint8_t (*transmit)(void * _lora_ctx, uint8_t* data, uint8_t length, uint16_t timeout);
    uint8_t (*receive)(void * _lora_ctx, uint8_t* data, uint8_t length); // called when something is in the receive buffer
    volatile uint8_t receive_ready_flag;
    void * lora_ctx;
} LoraDriver;


typedef void (*LoraPingReqHandler)(LoraEngine *engine,
                                   const LoraPingReq *msg,
                                   const LoraMetadata *meta);

typedef void (*LoraPingRespHandler)(LoraEngine *engine,
                                    const LoraPingResp *msg,
                                    const LoraMetadata *meta);

typedef void (*LoraDataReqHandler)(LoraEngine *engine,
                                   const LoraDataReq *msg,
                                   const LoraMetadata *meta);

typedef void (*LoraDataHandler)(LoraEngine *engine,
                                const LoraData *msg,
                                const LoraMetadata *meta);

typedef void (*LoraCommandReqHandler)(LoraEngine *engine,
                                      const LoraCommandReq *msg,
                                      const LoraMetadata *meta);

typedef void (*LoraCommandRespHandler)(LoraEngine *engine,
                                       const LoraCommandResp *msg,
                                       const LoraMetadata *meta);

typedef void (*LoraStreamRequestHandler)(LoraEngine *engine,
                                         const LoraStreamRequest *msg,
                                         const LoraMetadata *meta);

typedef void (*LoraStreamAnnounceHandler)(LoraEngine *engine,
                                          const LoraStreamAnnounce *msg,
                                          const LoraMetadata *meta);

typedef void (*LoraStreamAnnounceAckHandler)(LoraEngine *engine,
                                             const LoraStreamAnnounceAck *msg,
                                             const LoraMetadata *meta);

typedef void (*LoraStreamSequenceHandler)(LoraEngine *engine,
                                          const LoraStreamSequence *msg,
                                          const LoraMetadata *meta);

typedef void (*LoraStreamSequenceAckHandler)(LoraEngine *engine,
                                             const LoraStreamSequenceAck *msg,
                                             const LoraMetadata *meta);

typedef void (*LoraStreamCompleteHandler)(LoraEngine *engine,
                                          const LoraStreamComplete *msg,
                                          const LoraMetadata *meta);


struct _LoraEngine {
    LoraDriver *driver;
    NodeId local_id;

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
};

/**
*   initialize a LoraEngine. Provide a LoraDriver implementation.
*/
void lora_engine_init(LoraEngine *engine, LoraDriver *driver);

/**
*   send a LoraMessage over the LoraEngine.
*/
uint8_t lora_engine_send(LoraEngine *engine,
                         LoraMessage *msg,
                         uint16_t timeout);

/**
*   pass a LoraMessage to this engine for processing through 
*   the appropriate handler function.
*   ie this function routes a LoraMessage to the appropriate engine Handler function.
*/
void lora_engine_handle_message(LoraEngine *engine,
                                const LoraMessage *msg);


/**
* Main Loop for LoraEngine
*/
void lora_engine_loop(LoraEngine *engine);

// //////////////////////////////////////////////////////////////
// simple requests
uint8_t lora_engine_send_ping(LoraEngine *engine,
                              NodeId dest,
                              uint16_t timeout);