#include "lora_message_types.h"
#include "lora_engine.h"
#include "lora_codec.h"
#include "LoRa.h"
#include <stdint.h>

/*
* Creates our custom LoraDriver instance
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
                            NodeId id);

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
                            NodeId id);