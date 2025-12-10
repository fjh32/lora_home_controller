#include "lora_home_controller.h"

void lora_home_controller_interrupt_handler(LoRa *lora)
{

}

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
)
{
    *lora = newLoRaLongRange();

	lora->CS_port         = CS_port;
	lora->CS_pin          = CS_pin;
	lora->reset_port      = reset_port;
	lora->reset_pin       = reset_pin;
	lora->DIO0_port       = DIO0_port;
	lora->DIO0_pin        = DIO0_pin;
	lora->enable_port	   = enable_port;
	lora->enable_pin	   = enable_pin;
	lora->hSPIx           = hSPIx;

	uint8_t init_status = LoRa_init(lora);

	if(init_status != LORA_OK)
	{
        return 0;
	}
	else
	{
        return 1;
	}
}