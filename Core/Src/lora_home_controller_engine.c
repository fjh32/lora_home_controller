#include "lora_home_controller_engine.h"
#include "LoRa.h"
#include "lora_engine.h"
#include "lora_message_types.h"
#include "stm32g4xx_hal.h"
#include <string.h>

static uint8_t lora_home_driver_transmit(void * _lora_ctx, 
                                        uint8_t* data, 
                                        uint8_t length, 
                                        uint16_t timeout)
{
    return LoRa_single_transmit((LoRa *)_lora_ctx, data, length, timeout);
}

static uint8_t lora_home_driver_receive(void * _lora_ctx, 
                                        uint8_t* data, 
                                        uint8_t length)
{
    return LoRa_receive((LoRa *)_lora_ctx, data, length);
}

/**
* when I receive a ping request, what do I want to do about it?
* Reply with a ping response
* 
*/
static void my_simple_ping_req_handler(LoraEngine *engine,
                                   const LoraPingReq *msg,
                                   const LoraMetadata *meta)
{
    LoraMessage response;

    response.message_type = LORA_PING_RESPONSE;
    response.metadata.dest = meta->source;
    response.metadata.source = engine->local_id;
    response.payload.ping_req._reserved = 0;

    if(!lora_engine_send(engine, &response,  1000))
    {
        // Error sending LoraMessage PingResponse
    }
}

/**
* If we get a response, send another request, why not.
*/
static void my_simple_ping_resp_handler(LoraEngine *engine,
                                    const LoraPingResp *msg,
                                    const LoraMetadata *meta)
{
    HAL_Delay(1000);

    // initiate new request when we receive a response (to a request we've presumably already sent)
    LoraMessage request;

    request.message_type = LORA_PING_REQUEST;
    request.metadata.dest = meta->source;
    request.metadata.source = engine->local_id;
    request.payload.ping_req._reserved = 0;

    if(!lora_engine_send(engine, &request,  1000))
    {
        // Error sending LoraMessage PingResponse
    }
}

/*
*   Creates our custom LoraDriver instance
*/
uint8_t new_lora_home_driver(LoraDriver *driver, 
                            LoRa *lora_ptr,
                            GPIO_TypeDef*		CS_port,
                            uint16_t		CS_pin,
                            GPIO_TypeDef*		reset_port,
                            uint16_t		reset_pin,
                            GPIO_TypeDef*		DIO0_port,
                            uint16_t		DIO0_pin,
                            GPIO_TypeDef*		enable_port,
                            uint16_t		enable_pin,
                            SPI_HandleTypeDef*	hSPIx,
                            NodeId id)
{
    memset(driver, 0, sizeof(*driver));

    driver->local_id = id;
    *lora_ptr = newLoRaLongRange();

    lora_ptr->CS_port         = CS_port;
	lora_ptr->CS_pin          = CS_pin;
	lora_ptr->reset_port      = reset_port;
	lora_ptr->reset_pin       = reset_pin;
	lora_ptr->DIO0_port       = DIO0_port;
	lora_ptr->DIO0_pin        = DIO0_pin;
	lora_ptr->enable_port	   = enable_port;
	lora_ptr->enable_pin	   = enable_pin;
	lora_ptr->hSPIx           = hSPIx;

    uint8_t init_status = LoRa_init(lora_ptr);
	if(init_status != LORA_OK)
	{
        return 0;
    }

    driver->lora_ctx = (void *)lora_ptr;
    driver->receive_ready_flag = 0;
    driver->transmit = lora_home_driver_transmit;
    driver->receive = lora_home_driver_receive;

    return 1;
}

/*
*   Creates our custom LoraEngine instance
*/
uint8_t new_lora_home_engine(LoraEngine *engine, 
                            LoraDriver *driver, 
                            LoRa *lora_ptr,
                            GPIO_TypeDef*		CS_port,
                            uint16_t		CS_pin,
                            GPIO_TypeDef*		reset_port,
                            uint16_t		reset_pin,
                            GPIO_TypeDef*		DIO0_port,
                            uint16_t		DIO0_pin,
                            GPIO_TypeDef*		enable_port,
                            uint16_t		enable_pin,
                            SPI_HandleTypeDef*	hSPIx,
                            NodeId id)
{
    if (!new_lora_home_driver(driver, 
        lora_ptr,
        CS_port,
        CS_pin,
        reset_port,
        reset_pin,
        DIO0_port,
        DIO0_pin,
        enable_port,
        enable_pin,
        hSPIx,
        id))
    {
        return 0;
    }

    lora_engine_init(engine, driver);

    engine->local_id = id;

    engine->on_ping_req  = my_simple_ping_req_handler;
    engine->on_ping_resp = my_simple_ping_resp_handler;

    return 1;
}