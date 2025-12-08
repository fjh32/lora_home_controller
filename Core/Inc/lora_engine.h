#pragma once
#include "lora_message_types.h" 
#include "lora_codec.h"


// Function pointer typedefs
// typedef void (*LoraPingReqHandler)(const LoraPingReq *msg, const LoraMeta *meta, void *ctx);
// typedef void (*LoraPingRespHandler)(const LoraPingResp *msg, const LoraMeta *meta, void *ctx);
// typedef void (*LoraDataReqHandler)(const LoraDataReq *msg, const LoraMeta *meta, void *ctx);
// typedef void (*LoraDataRespHandler)(const LoraDataResp *msg, const LoraMeta *meta, void *ctx);
// typedef void (*LoraCommandReqHandler)(const LoraCommandReq *msg, const LoraMeta *meta, void *ctx);
// typedef void (*LoraCommandRespHandler)(const LoraCommandResp *msg, const LoraMeta *meta, void *ctx);

// typedef struct {
//     LoraPingReqHandler     on_ping_req;
//     LoraPingRespHandler    on_ping_resp;
//     LoraDataReqHandler     on_data_req;
//     LoraDataRespHandler    on_data_resp;
//     LoraCommandReqHandler  on_command_req;
//     LoraCommandRespHandler on_command_resp;
//     void *ctx;  // user context pointer
// } LoraHandlers;