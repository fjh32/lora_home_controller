#include "simple_ping_handler.h"
#include "LoRa.h"
#include "lora_engine.h"

static void handle_ping_req(const LoraPingReq *req,
                            const LoraMetadata *meta,
                            void *ctx)
{
    // (void)req; // unused

    LoraDriver *lora_driver = (LoraDriver *)ctx;

    // Build response message
    LoraMessage resp = {0};
    resp.message_type     = LORA_PING_RESPONSE;
    resp.metadata.source  = 1;   // or provided externally
    resp.metadata.dest    = meta->source;

    // Encode
    uint8_t buf[LORA_MAX_ENCODED_SIZE];
    size_t len = lora_encode(&resp, buf, sizeof(buf));
    if (len == 0)
        return; // encoding error

    // Transmit
    // LoRa_single_transmit(lora_hw, buf, (uint8_t)len, 200);
    lora_driver->transmit(buf, (uint8_t)len, 200);
}

static void handle_ping_resp(const LoraPingResp *resp,
                             const LoraMetadata *meta,
                             void *ctx)
{
    (void)resp;
    (void)ctx;

    // For now just acknowledge receipt; users can expand this.
    // You can add logging here later.
    (void)meta;
}


// ------------------------------
// Public initializer
// ------------------------------

void simple_ping_handler_init(LoraHandlers *handlers, LoraDriver *lora_driver)
{
    // Zero out handler table first
    memset(handlers, 0, sizeof(*handlers));

    // Fill only ping handlers
    handlers->on_ping_req  = handle_ping_req;
    handlers->on_ping_resp = handle_ping_resp;

    // ctx will be the LoRa hardware object
    handlers->ctx = lora_driver;
}
