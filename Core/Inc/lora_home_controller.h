#ifndef LORA_HOME_CONTROLLER_H
#define LORA_HOME_CONTROLLER_H

#include "LoRa.h"

/*
* Assume this will be called appropriately when the lora interrupt goes off
*/
void lora_home_controller_interrupt_handler(LoRa *lora);

/*
* Setup the lora instance + anything specific needed for 
* home controller.
* Returns 0 on err
*/
int setup_lora_home_controller(
    LoRa*           lora,
    GPIO_TypeDef*		CS_port,
	uint16_t		CS_pin,
	GPIO_TypeDef*		reset_port,
	uint16_t		reset_pin,
	GPIO_TypeDef*		DIO0_port,
	uint16_t		DIO0_pin,
	GPIO_TypeDef*		enable_port,
	uint16_t		enable_pin,
	SPI_HandleTypeDef*	hSPIx
);

#endif