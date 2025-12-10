#pragma once

#include <stdint.h>
#include "lora_message_types.h"
#include "lora_engine.h"
#include "lora_codec.h"

#include "LoRa.h"
#include <string.h>

// Forward-declare your hardware driver type
// typedef struct LoRa lora;

// Create a handler table for ping-only behavior
void simple_ping_handler_init(LoraHandlers *handlers, LoraDriver *lora_driver);